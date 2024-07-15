#pragma once

#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include "SshSession.h"
#include "DataInfo.h"
#include "NodeManager.h"

class DataFlowSsh;

// ÿ��dateflow������datainfo������sshsession
// cmd����ʱ�Զ�ɾ��datainfo��sshsession�������¼��dataflow��
class DataFlow {
public:
    struct FlowInfo {
        struct NodeManager::NodeInfo *client; // �Զ�����
        struct NodeManager::NodeInfo *server; // �Զ�����
        std::string band; // ����λ K M G
        uint32_t port; // port �Զ�ѡ�� ������0����
        uint32_t time; // s
        std::string type; // tcp or udp Сд
        DataFlowSsh* client_ssh;
        DataFlowSsh* server_ssh;
        uint32_t use_cout; // ���ڽ�����ɾ�����д洢�Ĵ�����Ϣ
    };

    static int32_t creat_data_flow(struct FlowInfo& info);
    static void get_all_data_flow(std::vector<struct FlowInfo> &info);
    static void close_all_data_flow(void);
    static int32_t close_data_flow_server(std::string server_ip); // ֻ�з���˱�����������ģ��ͻ��˱����п���������ֹͣ
    static uint32_t band_width_str_to_num(std::string band_witdh_str);
    static int32_t delete_data_flow(struct FlowInfo &info); // �Ƚ�����Ϣ���ܵ�������ӿ�

private:
    using FlowInfoPortMap = std::map<int32_t, struct FlowInfo>; // ��һ������Ϊport
    using FlowInfoServerMap = std::map<std::string, FlowInfoPortMap>; // ��һ������Ϊserverip
    static FlowInfoServerMap m_flow_info_map;
    static struct FlowInfo *add_data_flow(struct FlowInfo &info);
};

class DataFlowSsh : public SshSession {
public:
    DataFlowSsh(DataFlow::FlowInfo *info, NodeManager::NodeInfo *node);

    DataInfo* m_data_info = nullptr;
    struct DataFlow::FlowInfo *m_flow_info;
    void cmd_end(void) override;
    void read_echo(char* data)  override;
};

