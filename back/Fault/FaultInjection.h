#pragma once
#include <stdint.h>
#include <string>
#include "SshSession.h"

// 每次只能注入一个故障，不可重复使用
class NodeCrash : public SshSession {
public:
    using SshSession::SshSession;
    int32_t fault_injection(void);

    //int32_t network_congestion(std::string ip); // limit traffic of node
    //int32_t application_crash(std::string ip); // crash all app of this node
    //int32_t malicious_traffic(std::string ip_src, std::string ip_dst); // large udp from src to dst

    //int32_t poor_routing_decision(std::string ip);
    //int32_t inject_fault(std::string &ip);

};

class NetworkCongestion : public SshSession {
public:
    using SshSession::SshSession;
    int32_t fault_injection(void);
    int32_t recover_injection(void);
    void read_echo(char* data) override;
    bool m_inject_fault = false; // 撤销恢复不可同时发送
};

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

