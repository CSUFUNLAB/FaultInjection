#pragma once
#include <stdint.h>
#include <string>
#include "SshSession.h"

// 使用ssh注入故障，每次只能注入一个故障，不可重复使用
class FaultBase : public SshSession {
public:
    typedef enum {
        FIX_NODE,
        RANDOM_NODE,
        RANDOM_NODE_EXIT_AP, // 一些故障发生在ap节点会很麻烦
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

    // 如果使用FIX_NODE，需要设置node参数
    // 可以使用ssh node info获取随机的故障节点
    static FaultBase* get_fault(FaultType fault_type, FaultNodeType fault_node_type, uint32_t fault_node = 0);
    // send ssh后不自动delete自身，在recover里面处理
    void cmd_end(void) override {};
    virtual void fault_injection(void) {}; // 一定要覆写
    virtual void recover_injection(void); // 在此处删除自身

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

// App down 不需要ssh，但是还是需要node节点号
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

