#include "DataFlow.h"
#include "Log.h"
#include "SshSession.h"
#include "BeginTime.h"

#include <functional>
#include <cctype>
#include <mutex>

using namespace std;

DataFlow::FlowInfoServerMap DataFlow::m_flow_info_map;

int32_t DataFlow::creat_data_flow(struct FlowInfo& input_info)
{
    ERR_RETURN(input_info.client == nullptr || input_info.server == nullptr, -NO_NODE, "null node info");

    FlowInfo *info = add_data_flow(input_info);
    info->use_cout = 2;
    ERR_RETURN(info == nullptr, -NO_NODE, "channel exist");
    string udp_cmd = "";
    string len_cmd = "";
    if (info->type == "udp") {
        udp_cmd = " -u";
    } else { // tcp 过低的带宽要减小缓冲区
        if (info->band.c_str()[info->band.size() - 1] == 'K') {
            uint32_t band_val = atoi(info->band.c_str());
            len_cmd = string(" -l ") + to_string(band_val * 128);
        }
    }
    info->client_ssh = new DataFlowSsh(info, info->client);
    info->server_ssh = new DataFlowSsh(info, info->server);

    int32_t ret;
    ret = info->client_ssh->open();
    ERR_RETURN_DEBUG_PRINT(ret != NORMAL_OK, ret, "open src ssh[{}]", info->client->ip);

    ret = info->server_ssh->open();
    ERR_RETURN_DEBUG_PRINT(ret != NORMAL_OK, ret, "open dst ssh[{}]", info->server->ip);

    uint32_t band_width = band_width_str_to_num(info->band);
    info->client->output_band += band_width;
    info->server->input_band += band_width;

    if (info->server->server_fault) {
        LOG_INFO("server[{}] has app down fault, don't create server", info->server->ip);
        info->server_ssh->send_cmd("\n"); // 这里还是发一条消息，用于client server pair结束
    } else {
        info->server_ssh->send_cmd("iperf -se -i 1 -p " + to_string(info->port) + udp_cmd + "\n");
        while (info->server_ssh->m_send_cmd) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    if (info->client->server_fault) {
        LOG_INFO("client[{}] has app down fault, don't create client", info->client->ip);
        info->client_ssh->send_cmd("\n"); // 这里还是发一条消息，用于client server pair结束
    } else {
        info->client_ssh->send_cmd("iperf -ec " + info->server->ip
            + string(" -i 1 -p ") + to_string(info->port)
            + string(" -t ") + to_string(info->time)
            + string(" -b ") + info->band
            + len_cmd + udp_cmd + "\n");
    }

    return 0;
}

struct DataFlow::FlowInfo *DataFlow::add_data_flow(struct FlowInfo& info)
{
    ERR_RETURN(info.client == nullptr || info.server == nullptr, nullptr, "node not exist");
    ERR_RETURN(info.client->detected == false || info.server->detected == false, nullptr,
        "detected client[{}][{}] server[{}][{}]", info.client->ip, info.client->detected, info.server->ip, info.server->detected );

    info.port = 8081;

    auto server = m_flow_info_map.find(info.server->ip);
    if (server == m_flow_info_map.end()) {
        FlowInfoPortMap port_map = {{info.port, info}};
        m_flow_info_map.insert({info.server->ip, port_map});
        LOG_INFO("add flow {} -> {} : {} {} {}s", info.client->index, info.server->index, info.port, info.band, info.time);
        return &m_flow_info_map.find(info.server->ip)->second.find(info.port)->second;
    }
    auto client = server->second.find(info.port);
    while (client != server->second.end()) { // port自动选择的情况下，总能找到可新插入的data flow
        info.port++;
        client = server->second.find(info.port);
    }
    server->second.insert({info.port, info});
    LOG_INFO("add flow {} -> {} : {} {} {}s", info.client->index, info.server->index, info.port, info.band, info.time);
    return &server->second.find(info.port)->second;
}

int32_t DataFlow::delete_data_flow(struct FlowInfo &info)
{
    ERR_RETURN(info.client == nullptr || info.server == nullptr, -NO_NODE, "node not exist");
    ERR_RETURN(info.client->detected == false || info.server->detected == false, -NO_NODE,
        "detected client[{}][{}] server[{}][{}]", info.client->ip, info.client->detected, info.server->ip, info.server->detected );

    auto server = m_flow_info_map.find(info.server->ip);
    ERR_RETURN(server == m_flow_info_map.end(), -NO_EXIST_FLOW,
        "flow {} -> {} : {} not exist", info.client->ip, info.server->ip, info.port);

    auto client = server->second.find(info.port);
    ERR_RETURN(client == server->second.end(), -NO_EXIST_FLOW,
        "flow {} -> {} : {} not exist", info.client->ip, info.server->ip, info.port);

    ERR_RETURN(client->second.client->ip != info.client->ip, -NO_EXIST_FLOW,
        "flow {} -> {} : {} not exist", info.client->ip, info.server->ip, info.port);

    LOG_INFO("delete flow {} -> {} : {}", info.client->ip, info.server->ip, info.port);
    FlowInfo& save_info = client->second;
    uint32_t band_width = band_width_str_to_num(save_info.band);
    save_info.client->output_band -= band_width;
    save_info.server->input_band -= band_width;
    save_info.client_ssh->broken_cmd();
    save_info.server_ssh->broken_cmd();
    server->second.erase(client);
    return 0;
}

void DataFlow::close_all_data_flow(void)
{
    for (auto server = m_flow_info_map.begin(); server != m_flow_info_map.end(); server++) {
        for (auto client = server->second.begin(); client != server->second.end(); client++) {
            client->second.server_ssh->broken_cmd();
            client->second.client_ssh->broken_cmd();
        }
    }
}

// 删除记录的信息，只结束server，因为结束client无法诊断故障，不知道是正常结束还是故障结束
int32_t DataFlow::close_data_flow_server(string server_ip)
{
    auto server = m_flow_info_map.find(server_ip);
    if (server == m_flow_info_map.end()) {
        LOG_INFO("server[{}] not have data flow", server_ip);
        return 0;
    }
    for (auto client = server->second.begin(); client != server->second.end(); client++) {
        FlowInfo& save_info = client->second;
        LOG_INFO("delete flow server {} -> {} : {}", save_info.client->ip, save_info.server->ip, save_info.port);
        // 只需要关闭服务端，会自动删除flow
        save_info.server_ssh->broken_cmd();
    }
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

uint32_t DataFlow::band_width_str_to_num(std::string band_witdh_str)
{
    uint32_t k = 1;
    char unit = band_witdh_str.c_str()[band_witdh_str.size() - 1];
    if (unit == 'K') {
        k = 1;
    } else if (unit == 'M') {
        k = 1000;
    } else if (unit == 'G') {
        k = 1000000;
    }
    return atoi(band_witdh_str.c_str()) * k;
}

DataFlowSsh::DataFlowSsh(DataFlow::FlowInfo *info, NodeManager::NodeInfo *node) : SshSession(node)
{
    if (info->client == node) {
        if (info->type == "tcp") {
            m_data_info = new TcpClientDataInfo(info->client, info->server);
        } else {
            m_data_info = new UdpClientDataInfo(info->client, info->server);
        }
    } else {
        if (info->type == "tcp") {
            m_data_info = new TcpServerDataInfo(info->client, info->server);
        } else {
            m_data_info = new UdpServerDataInfo(info->client, info->server);
        }
    }
    m_flow_info = info;
    m_cout = info->time;
    m_data_info->m_begin_time = BeginTime::get_instance()->get_time();
}

void DataFlowSsh::read_echo(char* data)
{
    m_data_info->deal_iperf_echo(data);
}

void DataFlowSsh::cmd_end(void)
{
    delete m_data_info;
    DataFlowSsh::close();
    bool need_detele_flow = false;
    static mutex m_mtx;
    m_mtx.lock();
    // client 和 server 都结束的时候才删除表中存储的flow数据
    m_flow_info->use_cout--;
    if (m_flow_info->use_cout == 0) {
        need_detele_flow = true;
    }
    m_mtx.unlock();
    if (need_detele_flow) {
        DataFlow::delete_data_flow(*m_flow_info);
    }
    delete this;
}



