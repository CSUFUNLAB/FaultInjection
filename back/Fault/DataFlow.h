#pragma once

#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include <mutex>

#include "SshSession.h"
#include "DataInfo.h"
#include "NodeManager.h"

class DataFlowSsh : public SshSession {
public:
    DataFlowSsh(DataInfo::FlowId &flow_id, std::string &type, NodeManager::NodeInfo *node);

    DataInfo* m_data_info = nullptr;
    struct DataInfo::FlowId m_flow_id;
    void cmd_end(void) override;
    void read_echo(char* data)  override;
};

// ÿ��dateflow������datainfo������sshsession
// cmd����ʱ�Զ�ɾ��datainfo��sshsession�������¼��dataflow��
class DataFlow {
public:
    struct FlowInfo {
        struct NodeManager::NodeInfo *client;
        struct NodeManager::NodeInfo *server;
        std::string band; // ����λ K M G
        uint32_t port; // port �Զ�ѡ�� ������0����
        uint32_t time; // s
        std::string type; // tcp or udp Сд
        DataFlowSsh* client_ssh; // �Զ�����
        DataFlowSsh* server_ssh; // �Զ�����
        uint32_t use_cout; // ���ڽ�����ɾ�����д洢�Ĵ�����Ϣ
        bool begin; // flow��ssh�Ƿ�ʼ��δ��ʼ����ɾ��info
    };

    typedef enum IperfType {
        SAVE_FILE,
        ONLINE_PARSING, // Ϊ��SAVE_FILE����ѡ��Ŀǰ����֤����
    };

    IperfType m_iperf_type = SAVE_FILE;

    static DataFlow* get_instance(void);

    int32_t creat_data_flow(struct FlowInfo& info);
    void get_all_data_flow(std::vector<struct FlowInfo> &info);
    void close_all_data_flow(void);
    int32_t close_data_flow_server(int32_t server_node_num); // ֻ�з���˱�����������ģ��ͻ��˱����п���������ֹͣ
    int32_t delete_data_flow(DataInfo::FlowId &flow_id); // �Ƚ�����Ϣ���ܵ�������ӿ�
    uint32_t band_width_str_to_num(std::string band_witdh_str);
    bool detect_all_data_flow(void);

private:
    using FlowInfoPortMap = std::map<int32_t, struct FlowInfo>; // ��һ������Ϊport
    using FlowInfoServerMap = std::map<int32_t, FlowInfoPortMap>; // ��һ������Ϊserver node num
    FlowInfoServerMap m_flow_info_map;
    struct FlowInfo *add_data_flow(struct FlowInfo &info);
    void send_cmd_thread(FlowInfo* info);
    void save_file_cmd(FlowInfo* info);
    void online_parsing_cmd(FlowInfo* info);
    std::mutex m_mtx;
};

class DataFlowClient {
public:
    static void send(DataFlow::FlowInfo& info);
};
