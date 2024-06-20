#pragma once
#include <stdint.h>
#include <string>
#include "SshSession.h"

class FaultSsh : public SshSession {
public:
    FaultSsh(struct NodeManager::NodeInfo* node, std::string& para) : m_para(para), SshSession(node) {};
    std::string m_para;
    virtual int32_t fault_injection(void) = 0;
};

// 每次只能注入一个故障，不可重复使用
class NodeCrash : public FaultSsh {
public:
    using FaultSsh::FaultSsh;

    int32_t fault_injection(void) override;
    //int32_t network_congestion(std::string ip); // limit traffic of node
    //int32_t application_crash(std::string ip); // crash all app of this node
    //int32_t malicious_traffic(std::string ip_src, std::string ip_dst); // large udp from src to dst

    //int32_t poor_routing_decision(std::string ip);
    //int32_t inject_fault(std::string &ip);

};

class NetworkCongestion : public FaultSsh {
public:
    using FaultSsh::FaultSsh;

    int32_t fault_injection(void) override;
    int32_t recover_injection(void);
    void read_echo(char* data) override;
    bool m_inject_fault = false; // 撤销/恢复故障标识
};

FaultSsh* create_faultssh(int32_t node_num, std::string &type, std::string &para);


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

