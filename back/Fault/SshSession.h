#pragma once

#include <string>
#include <stdint.h>
#include <libssh/libssh.h>
#include <functional>
#include <map>

#include "NodeManager.h"

class SshSession {
public:
    SshSession(struct NodeManager::NodeInfo *node);

    using CmdEnd = std::function<void(void)>;
    using CharDeal = std::function<void(char* data)>;

    int32_t open(void);
    void send_cmd(const std::string &cmd); // 没有回复消息，不再需要发命令，会自动结束ssh
    void broken_cmd(void);
    void close(void); // 非阻塞read返回数据时close会崩溃

    virtual void cmd_end(void); // 覆写使用
    virtual void read_echo(char *data); // 覆写使用

    struct NodeManager::NodeInfo* m_node_info = nullptr;
    bool m_send_cmd = false; // 是否有待发送消息，发送后会置false
    int32_t m_no_data_count = 0; // 没有数据时持续等待次数，如果数据量太大会导致ssh消息堵住
    bool m_last_cmd = true; // 默认只发一条消息
    bool m_ssh_end = false; // ssh是否结束

private:
    std::string m_host;
    std::string m_username;
    std::string m_password;
    ssh_session m_session = nullptr;
    ssh_channel m_channel = nullptr;
    std::string m_cmd;

    int32_t m_nbytes;
    char buffer[1024];
    bool m_broken_cmd = false;

    struct Credit {
        std::string username;
        std::string password;
    };

    using CreditMap = std::map<const std::string, SshSession::Credit>;

    static CreditMap credit_map;
    static struct Credit error_credit;
    struct Credit &find_credit(const std::string &node_describe);

    void send_cmd_thread(void);
};

