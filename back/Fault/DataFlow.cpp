#include "DataFlow.h"
#include "Log.h"
#include "SshSession.h"

#include <functional>
#include <cctype>

using namespace std;

DataFlow::FlowInfoServerMap DataFlow::m_flow_info_map;

int32_t DataFlow::creat_data_flow(struct FlowInfo& info)
{
    ERR_RETURN(info.client == nullptr || info.server == nullptr, -NO_NODE, "null node info");

    int32_t ret = add_data_flow(info);
    ERR_RETURN(ret != NORMAL_OK, ret, "channel exist");
    string udp_cmd = "";
    string len_cmd = "";
    if (info.type == "udp") {
        udp_cmd = " -u";
    } else { // tcp 过低的带宽要减小缓冲区
        if (info.bind.c_str()[info.bind.size() - 1] == 'K') {
            uint32_t bind_val = atoi(info.bind.c_str());
            len_cmd = string(" -l ") + to_string(bind_val * 128);
        }
    }
    info.client_ssh = new DataFlowSsh(info.client, info.server, "client", info.type);
    info.server_ssh = new DataFlowSsh(info.server, info.client, "server", info.type);

    ret = info.client_ssh->open();
    ERR_RETURN_PRINT(ret != NORMAL_OK, ret, "open src ssh[{}]", info.client->ip);

    ret = info.server_ssh->open();
    ERR_RETURN_PRINT(ret != NORMAL_OK, ret, "open dst ssh[{}]", info.server->ip);

    info.server_ssh->m_no_data_count = info.time * 3 / 2;
    info.client_ssh->m_no_data_count = info.time * 3 / 2;

    info.server_ssh->send_cmd("iperf -se -i 1 -p " + to_string(info.port) + udp_cmd + "\n");
    while (info.server_ssh->m_send_cmd) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    info.client_ssh->send_cmd("iperf -ec " + info.server->ip
        + string(" -i 1 -p ") + to_string(info.port)
        + string(" -t ") + to_string(info.time)
        + string(" -b ") + info.bind
        + len_cmd + udp_cmd + "\n");

    return 0;
}

int32_t DataFlow::add_data_flow(struct FlowInfo& info)
{
    ERR_RETURN(info.client == nullptr || info.server == nullptr, -NO_NODE, "node not exist");
    ERR_RETURN(info.client->detected == false || info.server->detected == false, -NO_NODE,
        "detected client[{}][{}] server[{}][{}]", info.client->ip, info.client->detected, info.server->ip, info.server->detected );

    info.port = 8081;

    auto server = m_flow_info_map.find(info.server->ip);
    if (server == m_flow_info_map.end()) {
        FlowInfoPortMap port_map = {{info.port, info}};
        m_flow_info_map.insert({info.server->ip, port_map});
        return 0;
    }
    auto client = server->second.find(info.port);
    while (client == server->second.end()) { // port自动选择的情况下，总能找到可新插入的data flow
        info.port++;
        client = server->second.find(info.port);
    }
    server->second.insert({info.port, info});
    return 0;
}

int32_t DataFlow::delete_data_flow(struct FlowInfo &info)
{
    ERR_RETURN(info.client == nullptr || info.server == nullptr, -NO_NODE, "node not exist");
    ERR_RETURN(info.client->detected == false || info.server->detected == false, -NO_NODE,
        "detected client[{}][{}] server[{}][{}]", info.client->ip, info.client->detected, info.server->ip, info.server->detected );

    auto server = m_flow_info_map.find(info.server->ip);
    ERR_RETURN(server == m_flow_info_map.end(), -NO_EXIST_FLOW,
        "flow client[{}][{}] server[{}][{}] not exist", info.client->ip, info.client->detected, info.server->ip, info.server->detected);

    auto client = server->second.find(info.port);
    ERR_RETURN(client == server->second.end(), -NO_EXIST_FLOW,
        "flow client[{}][{}] server[{}][{}] not exist", info.client->ip, info.client->detected, info.server->ip, info.server->detected);

    client->second.client_ssh->broken_cmd();
    client->second.server_ssh->broken_cmd();
    server->second.erase(client);
    return 0;
}

// 删除client无法诊断故障，不知道是正常结束还是故障结束
int32_t DataFlow::delete_data_flow_server(struct FlowInfo& info)
{
    ERR_RETURN(info.client == nullptr || info.server == nullptr, -NO_NODE, "node not exist");
    ERR_RETURN(info.client->detected == false || info.server->detected == false, -NO_NODE,
        "detected client[{}][{}] server[{}][{}]", info.client->ip, info.client->detected, info.server->ip, info.server->detected );

    auto server = m_flow_info_map.find(info.server->ip);
    ERR_RETURN(server == m_flow_info_map.end(), -NO_EXIST_FLOW,
        "flow client[{}][{}] server[{}][{}] not exist", info.client->ip, info.client->detected, info.server->ip, info.server->detected);

    auto client = server->second.find(info.port);
    ERR_RETURN(client == server->second.end(), -NO_EXIST_FLOW,
        "flow client[{}][{}] server[{}][{}] not exist", info.client->ip, info.client->detected, info.server->ip, info.server->detected);

    client->second.server_ssh->broken_cmd();
    server->second.erase(client);
    return 0;
}

void DataFlow::get_all_data_flow(std::vector<struct FlowInfo>& info)
{
    for (auto server = m_flow_info_map.begin(); server != m_flow_info_map.end(); server++) {
        auto& server_map = server->second;
        for (auto client = server_map.begin(); client != server_map.end(); client++)
        info.push_back(client->second);
    }
}

DataFlowSsh::DataFlowSsh(NodeManager::NodeInfo *node, NodeManager::NodeInfo *pair_node, string type, string protocol) : SshSession(node)
{
    if (type == "client") {
        if (protocol == "tcp") {
            m_data_info = new TcpClientDataInfo(node, pair_node);
        } else if (protocol == "udp") {
            m_data_info = new UdpClientDataInfo(node, pair_node);
        }
    } else if (type == "server") {
        if (protocol == "tcp") {
            m_data_info = new TcpServerDataInfo(node, pair_node);
        } else if (protocol == "udp") {
            m_data_info = new UdpServerDataInfo(node, pair_node);
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



