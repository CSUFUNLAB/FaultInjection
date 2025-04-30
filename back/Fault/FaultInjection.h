#pragma once
#include <stdint.h>
#include <string>
#include "SshSession.h"

// ʹ��sshע����ϣ�ÿ��ֻ��ע��һ�����ϣ������ظ�ʹ��
class FaultBase : public SshSession {
public:
    typedef enum {
        FIX_NODE,
        RANDOM_NODE,
        RANDOM_NODE_EXIT_AP, // һЩ���Ϸ�����ap�ڵ����鷳
    } FaultNodeType;

    typedef enum {
        NORMAL,
        NODE_CRASH,
        APP_CRASH,
        CONGESTION,
        TRAFFIC,
        CPU_OVERLOADER,
        FIRE_WALL,
        SIGNAL,
        FAULT_BUTT,
    } FaultType;

    using SshSession::SshSession;

    // ���ʹ��FIX_NODE����Ҫ����node����
    // ����ʹ��ssh node info��ȡ����Ĺ��Ͻڵ�
    static FaultBase* get_fault(FaultType fault_type, FaultNodeType fault_node_type, uint32_t fault_node = 0);
    // send ssh���Զ�delete������recover���洦��
    void cmd_end(void) override {};
    virtual void fault_injection(void) {}; // һ��Ҫ��д
    virtual void recover_injection(void); // �ڴ˴�ɾ������

    static FaultType m_fault_type;
    static uint32_t m_fault_node;
    static std::string m_fault_str[FAULT_BUTT];
};

class NoFault : public FaultBase {
public:
    using FaultBase::FaultBase;
    void fault_injection(void) override {};
};

class NodeCrash : public FaultBase {
public:
    using FaultBase::FaultBase;
    void fault_injection(void) override;
};

// App down ����Ҫssh�����ǻ�����Ҫnode�ڵ��
class AppDown : public FaultBase {
public:
    using FaultBase::FaultBase;
    void fault_injection(void) override;
    void recover_injection(void) override;
};

class MaliciousTraffic : public FaultBase {
public:
    using FaultBase::FaultBase;
    void fault_injection(void) override;
};

class CpuOverLoad : public FaultBase {
public:
    using FaultBase::FaultBase;
    void fault_injection(void) override;
    void recover_injection(void) override;
    std::string bandwith_reduce(std::string band);
};

class FireWall : public FaultBase {
public:
    using FaultBase::FaultBase;
    void fault_injection(void) override;
    void recover_injection(void) override;
};

/*
class NetworkCongestionSsh : public SshSession {
public:
    NetworkCongestionSsh(NodeManager::NodeInfo* node, std::string *para, bool *is_fault)
        : SshSession(node), m_para(para), m_is_fault(is_fault) {};
    bool *m_is_fault;
    std::string *m_para;
    void read_echo(char* data) override;
};

class NetworkCongestion : public FaultBase {
public:
    using FaultBase::FaultBase;
    int32_t fault_injection(void) override;
    int32_t recover_injection(void) override;
};
*/

#if 0
class NodeCrash : public FaultInjection {
public:
    NodeCrash() {
        m_fault_cmd = "reboot";
    };
    int32_t inject(std::string &ip);
};

class NetworkCongestion : public FaultInjection {
public:
    int32_t inject(std::string &ip);
};
#endif

