#pragma once
#include <stdint.h>
#include <string>
#include "SshSession.h"

// 使用ssh注入故障，每次只能注入一个故障，不可重复使用
class FaultBase {
public:
    FaultBase(SshSession* ssh, std::string& para) : m_ssh(ssh), m_para(para) {};
    ~FaultBase();
    SshSession *m_ssh;
    std::string m_para;
    bool m_is_fault = false;

    static FaultBase* create_fault(int32_t node_num, std::string& type, std::string& para);
    virtual int32_t fault_injection(void) = 0;
    virtual int32_t recover_injection(void) = 0;
};

class NodeCrash : public FaultBase {
public:
    using FaultBase::FaultBase;

    int32_t fault_injection(void) override;
    int32_t recover_injection(void) override { return 0; }; // crash故障原理是重启，无需恢复
};

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

// App down 不需要ssh
class AppDown : public FaultBase {
public:
    using FaultBase::FaultBase;
    int32_t fault_injection(void) override;
    int32_t recover_injection(void) override;
};

FaultBase* create_faultssh(int32_t node_num, std::string &type, std::string &para);

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

