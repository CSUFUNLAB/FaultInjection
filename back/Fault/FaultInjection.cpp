#include "FaultInjection.h"
#include "NodeManager.h"
#include "Log.h"

using namespace std;

int32_t NodeCrash::fault_injection(void)
{
    int32_t ret = open();
    ERR_RETURN(ret != 0, -1, "open ssh failed");
    send_cmd("reboot\n");
    return 0;
}

int32_t NetworkCongestion::fault_injection(void)
{
    m_last_cmd = false;
    m_inject_fault = true;
    int32_t ret = open();
    ERR_RETURN(ret != 0, -1, "open ssh failed");
    send_cmd("modprobe iptable_mangle\n");
    return 0;
}

void NetworkCongestion::read_echo(char* data)
{
    if (m_inject_fault) {
        send_cmd(string("tc qdisc add dev ") + m_node_info->dev + string(" root tbf rate ") + m_para + string("bit latency 50ms burst 15k\n"));
    }
}

int32_t NetworkCongestion::recover_injection(void)
{
    m_inject_fault = false;
    int32_t ret = open();
    ERR_RETURN(ret != 0, -1, "open ssh failed");
    send_cmd(string("tc qdisc del dev ") + m_node_info->dev + string(" root\n"));
    return 0;
}

FaultSsh* create_faultssh(int32_t node_num, std::string &type, std::string &para)
{
    NodeManager::NodeInfo *node = NodeManager::get_node_info(node_num);
    ERR_RETURN(node == nullptr, nullptr, "get node info failed");
    if (type == "congest") {
        return new NetworkCongestion(node, para);
    }
    if (type == "nodedown") {
        return new NodeCrash(node, para);
    }
    return nullptr;
}

