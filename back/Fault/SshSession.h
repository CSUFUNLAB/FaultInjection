#pragma once

#include <string>
#include <stdint.h>
#include <libssh/libssh.h>
#include <functional>
#include <map>
#include <queue>
#include <mutex>

#include "NodeManager.h"

class SshSession {
public:
    SshSession(struct NodeManager::NodeInfo *node);
    // ~SshSession(); 消息结束自动释放，如果需要手动释放，可以通过消息结束命令

    using CmdEnd = std::function<void(void)>;
    using CharDeal = std::function<void(char* data)>;

    int32_t open_and_send(void); // open 会同时创建send_thread
    int32_t only_open(void); // only open 不会创建send_thread
    void send_cmd(const std::string &cmd); // 没有回复消息，不再需要发命令，会自动结束ssh
    void queue_send_cmd(const std::string &cmd); // 用于大量命令发送的场景
    void send_thread(void);
    void broken_cmd(void);
    void close(void); // 非阻塞read返回数据时close会崩溃

    virtual void cmd_end(void); // 覆写使用
    virtual void read_echo(char *data); // 覆写使用

    struct NodeManager::NodeInfo* m_node_info = nullptr; // ssh目标节点信息
    int32_t m_nbytes; // 读取ssh返回字节数
    int32_t m_wait = 0; // 至少等待时间
    bool m_send_cmd = false; // 是否有待发送消息，发送后会置false
    bool m_last_cmd = true; // 默认只发一条消息
    bool m_always_read = false; // 即便没有返回数据，也会调用一次read echo
    uint32_t m_heart = 0;
    bool m_broken_cmd = false;

    typedef enum {
        SHELL_SSH, // 打开shell，有交互
        QUEUE_CMD, // 将需要发送的命令放入m_cmd_queue逐个发送，无回显
        EXEC_CMD, // 只发送一个命令，一般用于脚本，无回显
    } SshSendType ;

    SshSendType m_send_type = SHELL_SSH;

    // python ssh 是用python脚本打开cmd然后使用windows的openssh
    // 不需要再次调用open send等函数
    // 不支持QUEUE_CMD，SHELL_SSH仅读取一条命令的回复
    // 尽量不在代码中写过于复杂的命令行逻辑，同时windows的cmd脚本太难写了，用python脚本
    void python_ssh(std::string cmd);

private:
    std::string m_host;
    std::string m_username;
    std::string m_password;
    ssh_session m_session = nullptr;
    ssh_channel m_channel = nullptr;
    std::string m_cmd;

    std::mutex m_mtx;
    std::queue<std::string> m_cmd_queue;

    char buffer[1024];

    struct Credit {
        std::string username;
        std::string password;
    };

    using CreditMap = std::map<const std::string, SshSession::Credit>;

    CreditMap credit_map;
    static struct Credit error_credit;
    struct Credit &find_credit(const std::string &node_describe);

    void ssh_begin_read(void); // 读取ssh建立时一大段文字
    void send_cmd_thread(void);
    void only_send_cmd_thread(void);
    void python_ssh_thread(void);
    void exec_cmd_thread(void);
    void initialize_credit_map();
};

