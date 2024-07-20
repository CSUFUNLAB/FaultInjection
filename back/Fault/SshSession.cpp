#include "SshSession.h"
#include "Log.h"

#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

SshSession::CreditMap SshSession::credit_map = {
    {
        "linux server",
        {
            "remote", // username
            "MeanZhao%10", // password
        },
    },
    {
        "antsdr",
        {
            "root", // username
            "openwifi", // password
        },
    },
    {
        "orangepi",
        {
            "orangepi", // username
            "orangepi", // password
        },
    },
};

struct SshSession::Credit SshSession::error_credit = {
    "none", // username
    "none", // password
};

SshSession::SshSession(struct NodeManager::NodeInfo *node)
{
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

int32_t SshSession::open(void)
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
    ERR_RETURN_PRINT(rc != SSH_OK, -NORMAL_ERR, "open ssh channel[{}]", m_node_info->index);

    if (m_only_send) {
        std::thread channel_chread(&SshSession::only_send_cmd_thread, this);
        channel_chread.detach();
    } else {
        std::thread channel_chread(&SshSession::send_cmd_thread, this);
        channel_chread.detach();
    }

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
    if (m_only_send) {
        std::thread channel_chread(&SshSession::only_send_cmd_thread, this);
        channel_chread.detach();
    } else {
        std::thread channel_chread(&SshSession::send_cmd_thread, this);
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
        if (m_cout > 0) {
            m_cout -= 1;
        }
        if (m_always_read || m_nbytes > 0) {
            read_echo(buffer);
        }
        // 如果带宽占满，可能会导致ssh消息堵住，从而直接结束，但是这也算一个故障
        if (m_last_cmd && m_nbytes == 0 && m_cout <= 0) {
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

void SshSession::only_send_cmd(const std::string &cmd)
{
    m_mtx.lock();
    m_cmd_queue.push(cmd);
    m_mtx.unlock();
}

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

