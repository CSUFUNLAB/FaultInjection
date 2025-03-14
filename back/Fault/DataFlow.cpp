#include "DataFlow.h"
#include "Log.h"
#include "SshSession.h"
#include "BeginTime.h"
#include "DataInjectInterface.h"

#include <functional>
#include <cctype>

using namespace std;

DataFlow* DataFlow::get_instance(void)
{
    static DataFlow* p = new DataFlow();
    return p;
}

int32_t DataFlow::creat_data_flow(struct FlowInfo& input_info)
{
    ERR_RETURN(input_info.client == nullptr || input_info.server == nullptr, -NO_NODE, "null node info");

    m_mtx.lock();
    FlowInfo *info = add_data_flow(input_info);
    if (info == nullptr) {
        m_mtx.unlock();
        return -NO_NODE;
    }
    info->use_cout = 2;
    info->begin = false;
    DataInfo::FlowId flow_id = {
        info->client->index,
        info->server->index,
        info->time,
        info->port,
        &(info->use_cout),
    };
    info->client_ssh = new DataFlowSsh(flow_id, info->client);
    info->server_ssh = new DataFlowSsh(flow_id, info->server);
    m_mtx.unlock();

    #if 0
    int32_t client_ret = info->client_ssh->only_open();
    int32_t server_ret = info->server_ssh->only_open();
    if (client_ret != NORMAL_OK || server_ret != NORMAL_OK) {
        LOG_ERR("open src ssh[{}][{}]: {}", info->client->index, info->client->ip, client_ret);
        LOG_ERR("open src ssh[{}][{}]: {}", info->server->index, info->server->ip, server_ret);
        info->begin = true; // ��delete_data_flow����ɾ��
        delete info->client_ssh;
        delete info->server_ssh;
        DataFlow::delete_data_flow(flow_id);
        return NORMAL_ERR;
    }

    DataFlowClient::send(*info);

    info->client_ssh->send_thread();
    info->server_ssh->send_thread();

    std::thread channel_chread(&DataFlow::send_cmd_thread, DataFlow::get_instance(), info);
    channel_chread.detach();
    #endif

    save_file_cmd(info);

    return 0;
}

void DataFlow::send_cmd_thread(FlowInfo* info)
{
    if (m_iperf_type == SAVE_FILE) {
        save_file_cmd(info);
    } else if (m_iperf_type == ONLINE_PARSING) {
        online_parsing_cmd(info);
    } else {
        LOG_ERR("err iperf type[{}]", (int)(m_iperf_type));
    }
}

void DataFlow::save_file_cmd(FlowInfo* info)
{
    // app down�����ڴ˴���
    // m_wait����nohup����ʱ�䣬Ҳ��ssh����ʱ��
    info->client_ssh->m_wait = info->time + 2; // ��2s��ֹ���
    info->server_ssh->m_wait = info->time + 2 + 7; // server�Ƚ������Ӿ��鿴��Ҫ����7s

    string udp_cmd = "";
    string len_cmd = "";
    // clientnodeindex_servernode_index_isserver
    string base_file_name = string("data/") + to_string(info->client->index) + string("_") + to_string(info->server->index);
    string client_file_name = base_file_name + string("_0.json");
    string server_file_name = base_file_name + string("_1.json");
    if (info->type == "udp") {
        udp_cmd = " -u";
    } else { // tcp ���͵Ĵ���Ҫ��С������
        if (info->band.c_str()[info->band.size() - 1] == 'K') {
            uint32_t band_val = atoi(info->band.c_str());
            len_cmd = string(" -l ") + to_string(band_val * 128);
        }
    }
    info->server_ssh->python_ssh(
        string("nohup timeout ") + to_string(info->server_ssh->m_wait) + string(" \\\"") +
        string(" iperf3 -s -i 1 -p ") + to_string(info->port) +
        string(" -J --logfile ") + server_file_name +
        string("\\\""));

    while (info->server_ssh->m_send_cmd) { // ���б�����ܲ���server_ssh�����ڣ���info��������
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    info->client_ssh->python_ssh(
        string("nohup timeout ") + to_string(info->client_ssh->m_wait) + string(" \\\"") +
        string("iperf3 -c ") + info->server->ip +
        string(" -i 1 -p ") + to_string(info->port) +
        string(" -t ") + to_string(info->time) +
        string(" -b ") + info->band +
        len_cmd +
        udp_cmd +
        string(" -J --logfile ") + client_file_name +
        string("\\\""));

    info->begin = true;
}

void DataFlow::online_parsing_cmd(FlowInfo *info)
{
    string udp_cmd = "";
    string len_cmd = "";
    if (info->type == "udp") {
        udp_cmd = " -u";
    } else { // tcp ���͵Ĵ���Ҫ��С������
        if (info->band.c_str()[info->band.size() - 1] == 'K') {
            uint32_t band_val = atoi(info->band.c_str());
            len_cmd = string(" -l ") + to_string(band_val * 128);
        }
    }
    uint32_t band_width = band_width_str_to_num(info->band);
    info->client->output_band += band_width;
    info->server->input_band += band_width;

    // ����������ڽڵ�����app�������ǲ�̫�����
    if (info->server->server_fault) {
        LOG_INFO("server[{}] has app down fault, don't create server", info->server->index);
        info->server_ssh->send_cmd("\n"); // ���ﻹ�Ƿ�һ����Ϣ������client server pair����
    } else {
        info->server_ssh->send_cmd("iperf -se -i 1 -p " + to_string(info->port) + udp_cmd + "\n");
        while (info->server_ssh->m_send_cmd) { // ���б�����ܲ���server_ssh�����ڣ���info��������
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    if (info->client->server_fault) {
        LOG_INFO("client[{}] has app down fault, don't create client", info->client->index);
        info->client_ssh->send_cmd("\n"); // ���ﻹ�Ƿ�һ����Ϣ������client server pair����
    } else {
        info->client_ssh->send_cmd("iperf -ec " + info->server->ip
            + string(" -i 1 -p ") + to_string(info->port)
            + string(" -t ") + to_string(info->time)
            + string(" -b ") + info->band
            + len_cmd + udp_cmd + "\n");
    }
    info->begin = true;
}

struct DataFlow::FlowInfo *DataFlow::add_data_flow(struct FlowInfo& info)
{
    ERR_RETURN(info.client == nullptr || info.server == nullptr, nullptr, "node not exist");
    ERR_RETURN(info.client->detected == false || info.server->detected == false, nullptr,
        "client[{}][{}] or server[{}][{}] not detected", info.client->index, info.client->detected, info.server->index, info.server->detected );

    info.port = 8081;

    auto flow_by_server = m_flow_info_map.find(info.server->index);
    if (flow_by_server == m_flow_info_map.end()) {
        FlowInfoPortMap port_map = {{info.port, info}};
        m_flow_info_map.insert({info.server->index, port_map});
        LOG_INFO("add flow {} -> {} : {} {} {} {}s", info.client->index, info.server->index, info.port, info.type, info.band, info.time);
        return &m_flow_info_map.find(info.server->index)->second.find(info.port)->second;
    }
    auto info_it = flow_by_server->second.find(info.port);
    uint32_t pair_node_num = 0;
    if (info.server->index == 1) {
        pair_node_num = 2;
    } else if (info.server->index == 2) {
        pair_node_num = 1;
    }
    // ap����������eth��ap�����Ƕ˹��ÿ�
    FlowInfoServerMap::iterator pair_node_flow;
    FlowInfoPortMap::iterator pair_node_info_it;
    bool consider_pair = false;
    if (pair_node_num != 0) {
        pair_node_flow = m_flow_info_map.find(pair_node_num);
        if (pair_node_flow != m_flow_info_map.end()) {
            consider_pair = true;
            pair_node_info_it = pair_node_flow->second.find(info.port);
        }
    }
    while (info_it != flow_by_server->second.end() ||
            (consider_pair && pair_node_info_it != pair_node_flow->second.end())) { // port�Զ�ѡ�������£������ҵ����²����data flow
        info.port++;
        info_it = flow_by_server->second.find(info.port);
        if (consider_pair) {
            pair_node_info_it = pair_node_flow->second.find(info.port);
        }
    }

    flow_by_server->second.insert({info.port, info});
    LOG_INFO("add flow {} -> {} : {} {} {} {}s", info.client->index, info.server->index, info.port, info.type, info.band, info.time);
    return &flow_by_server->second.find(info.port)->second;
}

int32_t DataFlow::delete_data_flow(DataInfo::FlowId &flow_id)
{
    m_mtx.lock();
    do {
        auto flow_by_server = m_flow_info_map.find(flow_id.server_node_num);
        if (flow_by_server == m_flow_info_map.end()) {
            break;
        }
        auto info_it = flow_by_server->second.find(flow_id.port);
        if (info_it == flow_by_server->second.end()) {
            break;
        }
        if (info_it->second.client->index != flow_id.client_node_num) {
            break;
        }
        LOG_INFO("delete flow {} -> {} : {} {} {} {}s",
            flow_id.client_node_num, flow_id.server_node_num, flow_id.port, info_it->second.type, info_it->second.band, info_it->second.time);
        FlowInfo& info = info_it->second;
        uint32_t band_width = band_width_str_to_num(info.band);
        info.client->output_band -= band_width;
        info.server->input_band -= band_width;
        flow_by_server->second.erase(info_it);
        m_mtx.unlock();
        return 0;

    } while (0);

    m_mtx.unlock();
    LOG_ERR("flow {} -> {} : {} not exist", flow_id.client_node_num, flow_id.server_node_num, flow_id.port);
    return -NO_EXIST_FLOW;
}

void DataFlow::close_all_data_flow(void)
{
    LOG_INFO("close all data flow");
    for (auto flow_by_server = m_flow_info_map.begin(); flow_by_server != m_flow_info_map.end(); flow_by_server++) {
        for (auto info_it = flow_by_server->second.begin(); info_it != flow_by_server->second.end(); info_it++) {
            FlowInfo& info = info_it->second;
            LOG_INFO("close flow {} -> {} : {} {} {} {}s", info.client->index, info.server->index, info.port, info.type, info.band, info.time);
            info.server_ssh->broken_cmd();
            info.client_ssh->broken_cmd();
        }
    }
}

// ���Ժ�����Ŀǰ����ɾ���ɾ�band�����
bool DataFlow::detect_all_data_flow(void)
{
    int32_t cout = 0;
    bool ret = false;
    for (auto& node : NodeManager::m_node_info_list) {
        while (node.input_band > 0 || node.output_band > 0) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            LOG_INFO("node[{}] {}:{}", node.index, node.input_band, node.output_band);
            cout++;
            if (cout > 30) {
                LOG_ERR("some flow still not close");
                for (auto flow_by_server = m_flow_info_map.begin(); flow_by_server != m_flow_info_map.end(); flow_by_server++) {
                    for (auto info_it = flow_by_server->second.begin(); info_it != flow_by_server->second.end(); info_it++) {
                        FlowInfo& info = info_it->second;
                        LOG_INFO("exist flow {} -> {} : {} {} {} {}s", info.client->index, info.server->index, info.port, info.type, info.band, info.time);
                        // ������Ȼ��֪��Ϊʲô��ûɾ����
                        uint32_t band_width = band_width_str_to_num(info.band);
                        info.client->output_band -= band_width;
                        info.server->input_band -= band_width;
                        // ��״��broken_cmd����û�йص�ssh������û�д��������delete���̣����ܻ��ڴ�й©
                        info.client_ssh->broken_cmd();
                        info.server_ssh->broken_cmd();
                        while (!info.begin) { // ��begin֮ǰerase client���ܻᵼ��send_cmd_thread��ָ��
                            std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        }
                        flow_by_server->second.erase(info_it);
                        ret = true;
                    }
                }
                cout = 0;
            }
        }
    }
    return ret;
}

// ɾ����¼����Ϣ��ֻ����server����Ϊ����client�޷���Ϲ��ϣ���֪���������������ǹ��Ͻ���
int32_t DataFlow::close_data_flow_server(int32_t server_node_num)
{
    auto flow_by_server = m_flow_info_map.find(server_node_num);
    if (flow_by_server == m_flow_info_map.end()) {
        LOG_INFO("server[{}] not have data flow", server_node_num);
        return 0;
    }
    for (auto info_it = flow_by_server->second.begin(); info_it != flow_by_server->second.end(); info_it++) {
        FlowInfo& info = info_it->second;
        LOG_INFO("delete flow only server {} -> {} : {} {} {} {}s",
            info.client->index, info.server->index, info.port, info.type, info.band, info.time);
        // ֻ��Ҫ�رշ���ˣ����Զ�ɾ��flow
        info.server_ssh->broken_cmd();
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

void DataFlowSsh::cmd_end(void)
{
    DataFlowSsh::close();
    bool need_detele_flow = false;
    static mutex mtx;
    mtx.lock();
    // client �� server ��������ʱ���ɾ�����д洢��flow����
    (*m_flow_id.use_cout)--;
    if ((*m_flow_id.use_cout) == 0) {
        need_detele_flow = true;
    }
    mtx.unlock();
    if (need_detele_flow) {
        DataFlow::get_instance()->delete_data_flow(m_flow_id);
    }
    delete this;
}



