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
    // ~SshSession(); ��Ϣ�����Զ��ͷţ������Ҫ�ֶ��ͷţ�����ͨ����Ϣ��������

    using CmdEnd = std::function<void(void)>;
    using CharDeal = std::function<void(char* data)>;

    int32_t open_and_send(void); // open ��ͬʱ����send_thread
    int32_t only_open(void); // only open ���ᴴ��send_thread
    void send_cmd(const std::string &cmd); // û�лظ���Ϣ��������Ҫ��������Զ�����ssh
    void queue_send_cmd(const std::string &cmd); // ���ڴ�������͵ĳ���
    void send_thread(void);
    void broken_cmd(void);
    void close(void); // ������read��������ʱclose�����

    virtual void cmd_end(void); // ��дʹ��
    virtual void read_echo(char *data); // ��дʹ��

    struct NodeManager::NodeInfo* m_node_info = nullptr; // sshĿ��ڵ���Ϣ
    int32_t m_nbytes; // ��ȡssh�����ֽ���
    int32_t m_wait = 0; // ���ٵȴ�ʱ��
    bool m_send_cmd = false; // �Ƿ��д�������Ϣ�����ͺ����false
    bool m_last_cmd = true; // Ĭ��ֻ��һ����Ϣ
    bool m_always_read = false; // ����û�з������ݣ�Ҳ�����һ��read echo
    uint32_t m_heart = 0;
    bool m_broken_cmd = false;

    typedef enum {
        SHELL_SSH, // ��shell���н���
        QUEUE_CMD, // ����Ҫ���͵��������m_cmd_queue������ͣ��޻���
        EXEC_CMD, // ֻ����һ�����һ�����ڽű����޻���
    } SshSendType ;

    SshSendType m_send_type = SHELL_SSH;

    // python ssh ����python�ű���cmdȻ��ʹ��windows��openssh
    // ����Ҫ�ٴε���open send�Ⱥ���
    // ��֧��QUEUE_CMD��SHELL_SSH����ȡһ������Ļظ�
    // �������ڴ�����д���ڸ��ӵ��������߼���ͬʱwindows��cmd�ű�̫��д�ˣ���python�ű�
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

    void ssh_begin_read(void); // ��ȡssh����ʱһ�������
    void send_cmd_thread(void);
    void only_send_cmd_thread(void);
    void python_ssh_thread(void);
    void exec_cmd_thread(void);
    void initialize_credit_map();
};

