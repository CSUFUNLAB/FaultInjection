#include "FaultInjection.h"
#include "NodeManager.h"
#include "DataFlow.h"
#include "Log.h"

using namespace std;

FaultBase::~FaultBase()
{
    if (m_ssh != nullptr) {
        m_ssh->broken_cmd();
    }
}

int32_t NodeCrash::fault_injection(void)
{
    int32_t ret = m_ssh->open();
    ERR_RETURN(ret != 0, -1, "open ssh failed");
    m_ssh->send_cmd("reboot\n");
    return 0;
}

int32_t NodeCrash::recover_injection(void)
{
    return 0;
}

int32_t NetworkCongestion::fault_injection(void)
{
    m_is_fault = true;
    m_ssh->m_last_cmd = false;
    int32_t ret = m_ssh->open();
    ERR_RETURN(ret != 0, -1, "open ssh failed");
    m_ssh->send_cmd("modprobe iptable_mangle\n");
    return 0;
}

void NetworkCongestionSsh::read_echo(char* data)
{
    if (*m_is_fault) {
        send_cmd(string("tc qdisc add dev ") + m_node_info->dev + string(" root tbf rate ") + *m_para + string("bit latency 50ms burst 15k\n"));
    }
}

int32_t NetworkCongestion::recover_injection(void)
{
    m_is_fault = false;
    int32_t ret = m_ssh->open();
    ERR_RETURN(ret != 0, -1, "open ssh failed");
    m_ssh->send_cmd(string("tc qdisc del dev ") + m_ssh->m_node_info->dev + string(" root\n"));
    return 0;
}

int32_t AppDown::fault_injection(void)
{
    NodeManager::NodeInfo *node = NodeManager::get_node_info(atoi(m_para.c_str()));
    ERR_RETURN(node == nullptr, -NO_NODE, "get node info failed");
    LOG_INFO("node[{}] has fault app down", node->index);
    node->server_fault = true;
    return DataFlow::close_data_flow_server(node->ip);
}

int32_t AppDown::recover_injection(void)
{
    NodeManager::NodeInfo *node = NodeManager::get_node_info(atoi(m_para.c_str()));
    ERR_RETURN(node == nullptr, -NO_NODE, "get node info failed");
    node->server_fault = false;
    return 0;
}

FaultBase* FaultBase::create_fault(int32_t node_num, std::string &type, std::string &para)
{
    NodeManager::NodeInfo *node = NodeManager::get_node_info(node_num);
    ERR_RETURN(node == nullptr, nullptr, "get node info failed");
    SshSession* ssh = nullptr;
    LOG_INFO("create fault [{}] {} {}", node_num, type, para);
    if (type == "congest") {
        FaultBase *p = new NetworkCongestion(nullptr, para);
        ssh = new NetworkCongestionSsh(node, &p->m_para, &p->m_is_fault);
        p->m_ssh = ssh;
        return p;
    }
    if (type == "nodedown") {
        ssh = new SshSession(node);
        return new NodeCrash(ssh, para);
    }
    if (type == "appdown") {
        para = to_string(node_num);
        return new AppDown(nullptr, para);
    }
    return nullptr;
}

