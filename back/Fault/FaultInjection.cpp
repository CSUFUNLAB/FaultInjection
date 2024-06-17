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
        send_cmd("tc qdisc add dev eth0 root tbf rate 10mbit latency 50ms burst 15k\n");
    }
}

int32_t NetworkCongestion::recover_injection(void)
{
    m_inject_fault = false;
    int32_t ret = open();
    ERR_RETURN(ret != 0, -1, "open ssh failed");
    send_cmd("tc qdisc del dev eth0 root\n");
    return 0;
}

#if 0
int32_t FaultInjection::inject_fault(std::string &ip)
{
    ERR_RETURN(NodeManager::node_ip_exist(ip), NO_NODE, "node[{}] not exist", ip);

    m_ssh = new SshSession(ip);

    int32_t ret = m_ssh->open();
    ERR_RETURN_PRINT(ret != NORMAL_OK, NORMAL_ERR, "open src ssh[{}]", ip);

    SshSession::CmdEnd cmd_end_func = bind(&cmd_end, this);
    m_ssh->register_callback(cmd_end_func, nullptr);
    m_ssh->m_keep_read = m_ssh_keep_time;

    m_ssh->send_cmd(m_fault_cmd + "\n");
    return 0;
}

void FaultInjection::cmd_end(void)
{
    m_ssh->close();
    delete m_ssh;
    m_ssh = nullptr;
}

int32_t NodeCrash::inject(std::string &ip)
{
    return inject_fault(ip);

}

#endif
