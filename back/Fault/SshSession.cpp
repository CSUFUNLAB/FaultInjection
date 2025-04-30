#include "SshSession.h"
#include "Log.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <cpprest/filestream.h>
#include <cpprest/json.h>

using namespace std;


struct SshSession::Credit SshSession::error_credit = {
    "none", // username
    "none", // password
};

uint32_t SshSession::m_success_times = 0;

SshSession::SshSession(struct NodeManager::NodeInfo *node)
{
    //初始化credit_map
    initialize_credit_map();
    m_node_info = node;
    m_host = node->ip;
    Credit& credit = find_credit(node->describe);
    m_username = credit.username;
    m_password = credit.password;
}

struct SshSession::Credit& SshSession::find_credit(const string &node_describe)
{
    auto it = credit_map.find(node_describe);
    if (it == credit_map.end()) {
        return error_credit;
    }
    return it->second;
}

int32_t SshSession::open_and_send(void)
{
    only_open();
    send_thread();
    return 0;
}

int32_t SshSession::only_open(void)
{
    // 初始化SSH会话
    m_session = ssh_new();

    ERR_RETURN_DEBUG_PRINT(m_session == nullptr, -NORMAL_ERR, "create ssh session[{}][{}]", m_host, m_username);

    // 设置SSH服务器连接信息
    ssh_options_set(m_session, SSH_OPTIONS_HOST, m_host.c_str());
    ssh_options_set(m_session, SSH_OPTIONS_USER, m_username.c_str());
    // 连接SSH服务器
    int32_t rc = ssh_connect(m_session);
    ERR_RETURN_DEBUG_PRINT(rc != SSH_OK, -NORMAL_ERR, "connect ssh server[{}][{}]", m_host, m_username);

    // 认证用户
    rc = ssh_userauth_password(m_session, NULL, m_password.c_str());
    ERR_RETURN_DEBUG_PRINT(rc != SSH_OK, -NORMAL_ERR, "authenticate ssh user[{}][{}]", m_host, m_username);

    m_channel = ssh_channel_new(m_session);
    ERR_RETURN_DEBUG_PRINT(m_channel == nullptr, -NORMAL_ERR, "create ssh channel[{}][{}]", m_host, m_username);

    LOG_DEBUG("[{}][{}]ssh open", m_host, m_username);

    rc = ssh_channel_open_session(m_channel);
    ERR_RETURN_DEBUG_PRINT(rc != SSH_OK, -NORMAL_ERR, "open ssh channel[{}][{}]", m_host, m_username);

    return 0;
}

void SshSession::send_thread(void)
{
    if (m_send_type == QUEUE_CMD) {
        std::thread channel_chread(&SshSession::only_send_cmd_thread, this);
        channel_chread.detach();
    } else if (m_send_type == SHELL_CMD) {
        std::thread channel_chread(&SshSession::send_cmd_thread, this);
        channel_chread.detach();
    } else if (m_send_type == EXEC_CMD) {
        std::thread channel_chread(&SshSession::exec_cmd_thread, this);
        channel_chread.detach();
    }
}

void SshSession::ssh_begin_read(void)
{
    int32_t rc;

    rc = ssh_channel_request_pty(m_channel);
    ERR_RETURN(rc != SSH_OK, , "request ssh pty[{}][{}]", m_host, m_username);

    rc = ssh_channel_change_pty_size(m_channel, 80, 24);
    ERR_RETURN(rc != SSH_OK, , "change ssh pty[{}][{}]", m_host, m_username);

    rc = ssh_channel_request_shell(m_channel);
    ERR_RETURN_DEBUG_PRINT(rc != SSH_OK, , "request ssh shell[{}][{}]", m_host, m_username);

    char buffer[128];
    int32_t m_nbytes;
    while (1) {
        m_nbytes = ssh_channel_read_timeout(m_channel, buffer, sizeof(buffer), 0, 1500);
        if (m_nbytes <= 1) {
            break;
        }
        // 最后三个字符串为"~# "或者"~$ "
        if (buffer[m_nbytes - 2] == '#' || buffer[m_nbytes - 2] == '$') {
            if (m_nbytes == 2) {
                break;
            }
            if (buffer[m_nbytes - 3] == '~') {
                break;
            }
        }
    }
    LOG_DEBUG("[{}][{}]ssh ready", m_host, m_username);
}

// 在python通道下没有作用
void SshSession::close(void)
{
    LOG_DEBUG("close ssh[{}]", m_host);
    if (m_channel != nullptr) {
        ssh_channel_send_eof(m_channel);
        ssh_channel_close(m_channel);
        ssh_channel_free(m_channel);
    }
    if (m_session != nullptr) {
        ssh_disconnect(m_session);
        ssh_free(m_session);
    }
}


void SshSession::send_cmd(const std::string &cmd)
{
    m_cmd = cmd;
    m_send_cmd = true;
}

void SshSession::cmd_end(void)
{
    close();
    delete this;
}

void SshSession::read_echo(char* buff)
{
    return;
}

void SshSession::read_result(char* buff)
{
    read_echo(buff);
    string result = buff;
    size_t pos = result.find("cmd_success");
    if (pos != std::string::npos) {
        m_success_times++;
        LOG_INFO("[{}][{}]cmd_success[{}]: {}", m_node_info->index, m_node_info->ip, m_success_times, m_cmd.c_str());
        return;
    }
    pos = result.find("wait_timeout");
    if (pos != std::string::npos) {
        LOG_ERR("[{}][{}] {} : {}", m_node_info->index, m_node_info->ip, buff, m_cmd.c_str());
        return;
    }
    pos = result.find("EOF");
    if (pos != std::string::npos) {
        LOG_ERR("[{}][{}] {}: {}", m_node_info->index, m_node_info->ip, buff, m_cmd.c_str());
        return;
    }
    return;
}

void SshSession::broken_cmd(void)
{
    m_broken_cmd = true;
}

void SshSession::send_cmd_thread(void)
{
    int32_t ret;

    ssh_begin_read();

    while (!m_send_cmd) { // 必须等待一条消息
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    char buffer[1024];
    while (1) {
        if (m_send_cmd) {
            ret = ssh_channel_write(m_channel, m_cmd.c_str(), m_cmd.size());
            if (ret != m_cmd.size()) {
                LOG_ERR("[{}]send cmd[{}]: {}", m_node_info->index, ret, m_cmd);
            }
            LOG_DEBUG("[{}]send cmd[{}]: {}", m_node_info->index, ret, m_cmd);
            m_send_cmd = false;
        }
        m_nbytes = ssh_channel_read_timeout(m_channel, buffer, sizeof(buffer) - 1, 0, 2000);
        if (m_nbytes < 0 || m_nbytes > 1023) {
            LOG_ERR("[{}]ssh cmd read error[{}]", m_node_info->index, m_nbytes);
            break;
        }
        buffer[m_nbytes] = '\0'; // ssh_channel_read_timeout 不添加结束符
        LOG_DEBUG("[{}]{}: {}", m_node_info->index, m_nbytes, buffer);
        if (m_wait > 0) {
            m_wait -= 1;
        }
        if (m_always_read || m_nbytes > 0) {
            read_echo(buffer);
        }
        // 如果带宽占满，可能会导致ssh消息堵住，从而直接结束，但是这也算一个故障
        if (m_last_cmd && m_nbytes == 0 && m_wait <= 0) {
            LOG_INFO("[{}]ssh cmd nature end", m_node_info->index);
            break;
        }
        if (m_broken_cmd) {
            LOG_INFO("[{}]ssh cmd force end", m_node_info->index);
            break;
        }
    }

    cmd_end();

    return;
}

void SshSession::queue_send_cmd(const std::string &cmd)
{
    m_mtx.lock();
    m_cmd_queue.push(cmd);
    m_mtx.unlock();
}

// 用于纯发送，例如数据>>文件
void SshSession::only_send_cmd_thread(void)
{
    ssh_begin_read();
    string cmd;
    bool has_cmd = false;
    int32_t ret;

    while (1) {
        if (m_broken_cmd) {
            LOG_INFO("[{}]ssh cmd force end", m_node_info->index);
            break;
        }
        m_mtx.lock();
        if (!m_cmd_queue.empty()) {
            has_cmd = true;
            cmd = m_cmd_queue.front();
            LOG_DEBUG("{}", cmd);
            m_cmd_queue.pop();
        }
        m_mtx.unlock();
        if (has_cmd) {
            has_cmd = false;
            ret = ssh_channel_write(m_channel, cmd.c_str(), cmd.size());
            if (ret != cmd.size()) {
                LOG_ERR("[{}]send cmd[{}]: {}", m_node_info->index, ret, m_cmd);
            }
            continue;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    cmd_end();

    return;
}

void SshSession::exec_cmd_thread(void)
{
    int32_t ret;

    while (!m_send_cmd) { // 必须等待一条消息
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ret = ssh_channel_request_exec(m_channel, m_cmd.c_str());
    m_send_cmd = false;
    if (ret != 0) {
        LOG_ERR("[{}]send cmd[{}]: {}", m_node_info->index, ret, m_cmd);
    }
    if (m_wait > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(m_wait));
    }
    cmd_end();

    return;
}

// 初始化credit_map
void SshSession::initialize_credit_map() {
    // 打开配置文件
    std::ifstream file("./config.json");
    if (!file.is_open()) {
        throw std::runtime_error("File Not Found: config.json not found.");
    }

    // 读取文件到字符串流中
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    // 解析JSON数据
    auto jsonValue = web::json::value::parse(buffer.str());
    auto servers = jsonValue.at(U("servers")).as_array();

    // 填充credit_map
    for (const auto& server : servers) {
        std::string servername = utility::conversions::to_utf8string(server.at(U("servername")).as_string());
        std::string username = utility::conversions::to_utf8string(server.at(U("username")).as_string());
        std::string password = utility::conversions::to_utf8string(server.at(U("password")).as_string());
        Credit temp_credit;
        temp_credit.username = username;
        temp_credit.password = password;
        credit_map[servername] = temp_credit;
    }
}

void SshSession::python_ssh(std::string cmd)
{
    string username = m_is_root ? string("root") : m_username;
    string need_read;
    string echo_exe = string("echo cmd_has_exec;"); // 脚本里面会expect cmd_has_exec
    if (m_send_type == EXEC_CMD) {
        need_read = "0 ";
    } else if (m_send_type == SHELL_CMD) {
        need_read = "1 ";
        echo_exe = ""; // read会自动等待命令执行完毕，不需要echo等待了
    } else if (m_send_type == FIRST_CMD) {
        need_read = "2 ";
    } else if (m_send_type == NO_ECHO_CMD) {
        need_read = to_string(m_need_read) + string(" ");
        echo_exe = "";
    }
    int32_t password_loop = NodeManager::get_instance()->get_password_loop(m_node_info);
    if (m_is_root && password_loop == 0) {
        password_loop = 1;
    }
    string ssh_cmd = string("ssh ") + username + string("@") + m_host
        + string(" \\\"") + echo_exe + cmd + string("\\\"");
    m_cmd = string("python ../../script/ssh_connect.py ")
        + need_read
        + to_string(password_loop) + string(" ")
        + m_password
        + string(" \"") + ssh_cmd + string("\"");

    m_send_cmd = true;
    std::thread channel_chread(&SshSession::python_ssh_thread, this);
    channel_chread.detach();
}

void SshSession::python_scp(string& remote_path, string& local_path, int dir)
{
    string username = m_is_root ? string("root") : m_username;
    int32_t password_loop = NodeManager::get_instance()->get_password_loop(m_node_info);
    if (m_is_root) {
        password_loop += 1;
    }
    string scp_cmd;
    if (dir == 0) {
        scp_cmd = string("scp -r ") + username + string("@") + m_host + string(":") + remote_path
            + string(" ") + local_path;
    } else {
        scp_cmd = string("scp -r ") + local_path + string(" ")
            + username + string("@") + m_host + string(":") + remote_path;
    }
    m_cmd = string("python ../../script/ssh_connect.py ")
        + string("1 ") // scp一定到read等待命令执行完毕
        + to_string(password_loop) + string(" ")
        + m_password
        + string(" \"") + scp_cmd + string("\"");

    m_send_cmd = true;
    std::thread channel_chread(&SshSession::python_ssh_thread, this);
    channel_chread.detach();
}

void SshSession::python_ssh_thread(void)
{
    // LOG_INFO("[{}][{}] popen cmd: {}", m_node_info->index, m_node_info->ip, m_cmd.c_str());

    // 执行命令并打开管道
    LOG_DEBUG("[{}][{}] begin send", m_node_info->index, m_node_info->ip);

    FILE* pipe = _popen(m_cmd.c_str(), "r");
    if (!pipe) {
        LOG_ERR("[{}][{}] Failed to execute Python script!", m_node_info->index, m_node_info->ip);
        return;
    }

    // popen应该是在后台执行，所以这个地方没有时间节点

    // 无论如何把pipe中的缓冲读出来，否则直接关闭会报错
    // 如果不需要读取，不覆写read_echo即可
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        // LOG_INFO("[{}][{}]fgets: {}", m_node_info->index, m_node_info->ip, buffer);
        read_result(buffer);
    }

    m_send_cmd = false;

    // 关闭管道并检查返回值
    int exit_code = _pclose(pipe);
    if (exit_code != 0) {
        LOG_ERR("[{}][{}] Python script failed with exit code: {}", m_node_info->index, m_node_info->ip, exit_code);
    }
    if (m_wait > 0) {
        LOG_DEBUG("[{}][{}] wait {}", m_node_info->index, m_node_info->ip, m_wait);
        std::this_thread::sleep_for(std::chrono::seconds(m_wait));
    }
    LOG_DEBUG("[{}][{}] cmd end", m_node_info->index, m_node_info->ip);
    cmd_end();
}

void SshSession::wait_cmd(void)
{
    while(m_send_cmd) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

