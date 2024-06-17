#include "DataFlow.h"
#include "Log.h"
#include "SshSession.h"

#include <functional>
#include <cctype>

using namespace std;

DataFlow::FlowInfoMap DataFlow::m_flow_info_map;

int32_t DataFlow::creat_data_flow(struct FlowInfo& info)
{
    int32_t ret = add_data_flow(info);
    ERR_RETURN(ret != NORMAL_OK, ret, "channel exist");
    string udpcmd = "";
    if (info.type == "udp") {
        udpcmd = " -u";
    }
    info.client_ssh = new DataFlowSsh(info.client, "client", info.type);
    info.server_ssh = new DataFlowSsh(info.server, "server", info.type);

    ret = info.client_ssh->open();
    ERR_RETURN_PRINT(ret != NORMAL_OK, ret, "open src ssh[{}]", info.client->ip);

    ret = info.server_ssh->open();
    ERR_RETURN_PRINT(ret != NORMAL_OK, ret, "open dst ssh[{}]", info.server->ip);

    info.server_ssh->m_no_data_count = info.time * 3 / 2;
    info.client_ssh->m_no_data_count = info.time * 3 / 2;

    info.server_ssh->send_cmd("iperf -se -i 1 -p " + to_string(info.port) + udpcmd + "\n");
    while (info.server_ssh->m_send_cmd) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    info.client_ssh->send_cmd("iperf -ec " + info.server->ip
        + string(" -i 1 -p ") + to_string(info.port)
        + string(" -t ") + to_string(info.time)
        + string(" -b ") + info.bind
        + udpcmd + "\n");

    return 0;
}

int32_t DataFlow::add_data_flow(struct FlowInfo &info)
{
    ERR_RETURN(info.client->detected == false || info.server->detected == false, NO_NODE,
        "detected client[{}][{}] server[{}][{}]", info.client->ip, info.client->detected, info.server->ip, info.server->detected );
    string flow_key = info.client->ip + info.server->ip + to_string(info.port);
    auto it = m_flow_info_map.find(flow_key);
    ERR_RETURN_PRINT(it != m_flow_info_map.end(), EXIST_CMD, "add flow [{}]->[{}][{}]", info.client->ip, info.server->ip, info.port);
    m_flow_info_map.insert({flow_key, info});
    return 0;
}

int32_t DataFlow::delete_data_flow(struct FlowInfo &info)
{
    string flow_key = info.client->ip + info.server->ip + to_string(info.port);
    auto it = m_flow_info_map.find(flow_key);
    ERR_RETURN_PRINT(it == m_flow_info_map.end(), NORMAL_ERR, "delete flow [{}]->[{}][{}]", info.client->ip, info.server->ip, info.port);
    it->second.client_ssh->broken_cmd();
    it->second.server_ssh->broken_cmd();
    m_flow_info_map.erase(it);
}

// tcp 删除服务端之后似乎无法再发消息了，只有udp才可以继续发送
// tcp udp 一起发？
int32_t DataFlow::delete_data_flow_server(struct FlowInfo& info)
{
    string flow_key = info.client->ip + info.server->ip + to_string(info.port);
    auto it = m_flow_info_map.find(flow_key);
    ERR_RETURN(it == m_flow_info_map.end(), NORMAL_ERR, "no flow [{}]->[{}][{}]", info.client->ip, info.server->ip, info.port);
    LOG_INFO("delete flow server [{}]->[{}][{}]", info.client->ip, info.server->ip, info.port);
    it->second.server_ssh->broken_cmd();
    m_flow_info_map.erase(it);
}

void DataFlow::get_all_data_flow(std::vector<struct FlowInfo>& info)
{
    for (auto it = m_flow_info_map.begin(); it != m_flow_info_map.end(); it++) {
        info.push_back(it->second);
    }
}

DataFlowSsh::DataFlowSsh(struct NodeManager::NodeInfo *node, std::string type, std::string protocol) : SshSession(node)
{
    if (type == "client") {
        if (protocol == "tcp") {
            m_data_info = new TcpClientDataInfo(node);
        } else if (protocol == "udp") {
            m_data_info = new UdpClientDataInfo(node);
        }
    } else if (type == "server") {
        if (protocol == "tcp") {
            m_data_info = new TcpServerDataInfo(node);
        } else if (protocol == "udp") {
            m_data_info = new UdpServerDataInfo(node);
        }
    }
}

void DataFlowSsh::read_echo(char* data)
{
    m_data_info->deal_iperf_echo(data);
}

void DataFlowSsh::cmd_end(void)
{
    delete m_data_info;
    DataFlowSsh::close();
    delete this;
}



