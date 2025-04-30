#include "FaultInjection.h"
#include "NodeManager.h"
#include "DataFlow.h"
#include "RandomDataFlow.h"
#include "ShellScript.h"
#include "Log.h"

using namespace std;

std::string FaultBase::m_fault_str[FaultBase::FAULT_BUTT] = {
    "normal",
    "nodecrash",
    "appdown",
    "congestion",
    "traffic",
    "cpuoverload",
    "firewall",
    "signal",
};

FaultBase::FaultType FaultBase::m_fault_type = FaultBase::FAULT_BUTT;
uint32_t FaultBase::m_fault_node = 0;

FaultBase *FaultBase::get_fault(FaultType fault_type, FaultNodeType fault_node_type, uint32_t fault_node)
{
    switch (fault_node_type) {
        case FIX_NODE: break;
        case RANDOM_NODE:
            fault_node = RandomNode::get_instance()->get_random_node(); break;
        case RANDOM_NODE_EXIT_AP:
            fault_node = 3; // todo: 不写死
            while (fault_node == 3) {
                fault_node = RandomNode::get_instance()->get_random_node();
            }
            break;
        default: break;
    };
    m_fault_node = fault_node;
    m_fault_type = fault_type;
    LOG_INFO("------------------------ fault node {} ------------------------", fault_node);
    NodeManager::NodeInfo *fault_node_info = &NodeManager::m_node_info_list[fault_node];
    switch (fault_type) {
        case NORMAL: 
            LOG_INFO("------------------------ {} ------------------------", "normal");
            return new NoFault(fault_node_info);
        case NODE_CRASH: 
            LOG_INFO("------------------------ {} ------------------------", "node_crash");
            return new NodeCrash(fault_node_info);
        case APP_CRASH:
            LOG_INFO("------------------------ {} ------------------------", "app crash");
            return new AppDown(fault_node_info);
        case CONGESTION:
            LOG_INFO("------------------------ {} ------------------------", "congestion");
            return nullptr;
        case TRAFFIC:
            LOG_INFO("------------------------ {} ------------------------", "malicious traffic");
            return new MaliciousTraffic(fault_node_info);
        case CPU_OVERLOADER:
            LOG_INFO("------------------------ {} ------------------------", "cpu over loader");
            return new CpuOverLoad(fault_node_info);
        case FIRE_WALL:
            LOG_INFO("------------------------ {} ------------------------", "fire wall error");
            return new FireWall(fault_node_info);
        case SIGNAL:
            LOG_INFO("------------------------ {} ------------------------", "signal");
            return new FaultBase(fault_node_info);
        default: return nullptr;
    };
}

void FaultBase::recover_injection(void)
{
    delete this;
}

void NodeCrash::fault_injection(void)
{
    LOG_INFO("------------------------ {} ------------------------", "node_crash");
    m_is_root = true; // reboot需要root权限
    python_ssh("reboot");
}

// app down的原理是一个节点无法创建app
// 目前python ssh无法在运行中关闭iperf，其实可以kill但是看起来很麻烦
void AppDown::fault_injection(void)
{
    LOG_INFO("------------------------ {} ------------------------", "app crash");
    m_node_info->app_down = true;
    m_is_root = true;
    m_send_type = NO_ECHO_CMD;
    // 这个脚本-i每10s增加一个线程。-c是最多增加7个线程，
    // 一共执行时间-i * -c + -t
    python_ssh("nohup /home/orangepi/monitor_script/mem_leak.sh -m 200 -s 10 -i 1 -t 30 ^&");
}

void AppDown::recover_injection(void)
{
    python_ssh("pkill stress-ng");
    wait_cmd();
    NodeManager::get_instance()->deal_detected_node([](struct NodeManager::NodeInfo* node) {
        node->app_down = false;
    });
    delete this;
}

void MaliciousTraffic::fault_injection(void)
{
    LOG_INFO("------------------------ {} ------------------------", "malicious traffic");
    struct DataFlow::FlowInfo flow_info = {
        m_node_info,
        &NodeManager::m_node_info_list[3],
        "2M",
        0,
        120,
        "udp",
        false,
        nullptr,
        nullptr,
    };
    DataFlow::get_instance()->creat_data_flow(flow_info);
}

void CpuOverLoad::fault_injection(void)
{
    LOG_INFO("------------------------ {} ------------------------", "cpu over loader");
    m_node_info->cpu_over_loader = true;
    m_is_root = true;
    m_send_type = NO_ECHO_CMD;
    // 这个脚本-i每10s增加一个线程。-c是最多增加7个线程，
    // 一共执行时间-i * -c + -t
    python_ssh("nohup /home/orangepi/monitor_script/cpu_over_load.sh -c 7 -i 1 -t 40 ^&");
    // python_ssh("nohup /home/orangepi/monitor_script/cpu_over_load.sh -i 1 -c 7 -t 15 ^&");
}

void CpuOverLoad::recover_injection(void)
{
    python_ssh("pkill cpu_over_load.s");
    wait_cmd();
    NodeManager::get_instance()->deal_detected_node([](struct NodeManager::NodeInfo* node) {
        node->cpu_over_loader = false;
    });
    delete this;
}

std::string CpuOverLoad::bandwith_reduce(std::string band)
{
    GetCpuLoad* get_cpu_load = new GetCpuLoad(m_node_info);
    float cpu_load = get_cpu_load->get_cpu_load();
    delete get_cpu_load;
    if (cpu_load < 50) {
        return band;
    }
    uint32_t band_val = atoi(band.c_str());
    band_val = (uint32_t)(band_val * (100 - cpu_load) / 50);
    string output = to_string(band_val) + band.c_str()[band.size() - 1];
    LOG_INFO("reduce band to {}", output);
    return output;
}

void FireWall::fault_injection(void)
{
    m_is_root = true;
    uint32_t src_node;
    do {
        src_node = RandomNode::get_instance()->get_random_node();
    } while (src_node == 3 || src_node == m_node_info->index);
    LOG_INFO("------------------------ {} -> {} iptables error ------------------------", src_node, m_node_info->index);
    string cmd = string("iptables -A FORWARD -s ") +
        NodeManager::m_node_info_list[src_node].ip + string(" -d ") + m_node_info->ip + string(" -j DROP");
    python_ssh(cmd);
}

void FireWall::recover_injection(void)
{
    string cmd = string("iptables -F");
    python_ssh(cmd);
    wait_cmd();
    delete this;
}

/*
int32_t NetworkCongestion::fault_injection(void)
{
    m_is_fault = true;
    m_ssh->m_last_cmd = false;
    int32_t ret = m_ssh->open_and_send();
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
    int32_t ret = m_ssh->open_and_send();
    ERR_RETURN(ret != 0, -1, "open ssh failed");
    m_ssh->send_cmd(string("tc qdisc del dev ") + m_ssh->m_node_info->dev + string(" root\n"));
    return 0;
}
*/


