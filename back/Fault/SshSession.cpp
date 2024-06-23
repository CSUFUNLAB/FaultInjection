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
    // ��ʼ��SSH�Ự
    m_session = ssh_new();

    ERR_RETURN_PRINT(m_session == nullptr, -NORMAL_ERR, "create ssh session[{}][{}]", m_host, m_username);

    // ����SSH������������Ϣ
    ssh_options_set(m_session, SSH_OPTIONS_HOST, m_host.c_str());
    ssh_options_set(m_session, SSH_OPTIONS_USER, m_username.c_str());
    // ����SSH������
    int32_t rc = ssh_connect(m_session);
    ERR_RETURN_PRINT(rc != SSH_OK, -NORMAL_ERR, "connect ssh server[{}][{}]", m_host, m_username);

    // ��֤�û�
    rc = ssh_userauth_password(m_session, NULL, m_password.c_str());
    ERR_RETURN_PRINT(rc != SSH_OK, -NORMAL_ERR, "authenticate ssh user[{}][{}]", m_host, m_username);

    m_channel = ssh_channel_new(m_session);
    ERR_RETURN_PRINT(m_channel == nullptr, -NORMAL_ERR, "create ssh channel[{}][{}]", m_host, m_username);

    LOG_INFO("[{}][{}]ssh open", m_host, m_username);

    rc = ssh_channel_open_session(m_channel);
    ERR_RETURN_PRINT(rc != SSH_OK, -NORMAL_ERR, "open ssh channel[{}][{}]", m_host, m_username);

    rc = ssh_channel_request_pty(m_channel);
    ERR_RETURN(rc != SSH_OK, -NORMAL_ERR, "request ssh pty[{}][{}]", m_host, m_username);

    rc = ssh_channel_change_pty_size(m_channel, 80, 24);
    ERR_RETURN(rc != SSH_OK, -NORMAL_ERR, "change ssh pty[{}][{}]", m_host, m_username);

    rc = ssh_channel_request_shell(m_channel);
    ERR_RETURN_PRINT(rc != SSH_OK, -NORMAL_ERR, "request ssh shell[{}][{}]", m_host, m_username);


    // ��ȡSSH��������
    char buffer[128];
    int32_t m_nbytes;
    while (1) {
        m_nbytes = ssh_channel_read_timeout(m_channel, buffer, sizeof(buffer), 0, 2000);
        if (m_nbytes <= 1) {
            break;
        }
        // ��������ַ���Ϊ"~# "����"~$ "
        if (buffer[m_nbytes - 2] == '#' || buffer[m_nbytes - 2] == '$') {
            if (m_nbytes == 2) {
                break;
            }
            if (buffer[m_nbytes - 3] == '~') {
                break;
            }
        }
    }
    LOG_INFO("[{}][{}]ssh ready", m_host, m_username);

    std::thread channel_chread(&SshSession::send_cmd_thread, this);
    channel_chread.detach();

    return 0;
}

void SshSession::close(void)
{
    LOG_INFO("close ssh[{}]", m_host);
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

    while (!m_send_cmd) { // ����ȴ�һ����Ϣ
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    while (1) {
        if (m_send_cmd) {
            ret = ssh_channel_write(m_channel, m_cmd.c_str(), m_cmd.size());
            LOG_INFO("[{}][{}]send cmd[{}]: {}", m_host, m_username, ret, m_cmd);
            m_send_cmd = false;
        }
        m_nbytes = ssh_channel_read_timeout(m_channel, buffer, sizeof(buffer) - 1, 0, 1000);
        if (m_nbytes < 0 || m_nbytes > 1023) {
            LOG_INFO("[{}][{}]ssh cmd read error[{}]", m_host, m_username, m_nbytes);
            break;
        }
        buffer[m_nbytes] = '\0'; // ssh_channel_read_timeout ����ӽ�����
        //if (m_host == "192.168.13.5") {
            LOG_DEBUG("[{}]{}", m_host, buffer);
        //}
        read_echo(buffer);

        if (m_last_cmd && m_nbytes == 0 && m_no_data_count <= 0) {
            LOG_INFO("[{}][{}]ssh cmd nature end", m_host, m_username);
            break;
        }
        if (m_no_data_count > 0) {
            //LOG_DEBUG("[{}]keep{}", m_host, m_keep_read);
            m_no_data_count--;
            if (m_nbytes != 0) { // ����лظ���ʾ�������������ټ��ٶ���ȴ�
                m_no_data_count--; // ���ܻ����-1
            }
        }
        if (m_broken_cmd) {
            LOG_INFO("[{}][{}]ssh cmd force end", m_host, m_username);
            break;
        }
    }

    m_ssh_end = true;

    cmd_end();

    return;
}

