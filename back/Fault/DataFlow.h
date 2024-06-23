#pragma once

#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include "SshSession.h"
#include "DataInfo.h"
#include "NodeManager.h"

class DataFlowSsh : public SshSession {
public:
    DataFlowSsh(struct NodeManager::NodeInfo *node, std::string type, std::string protocol);

    DataInfo* m_data_info = nullptr;

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
        std::string bind; // ����λ K M G
        uint32_t port; // port �Զ�ѡ�� ������0����
        uint32_t time; // s
        std::string type; // tcp or udp Сд
        DataFlowSsh* client_ssh;
        DataFlowSsh* server_ssh;
    };

    static int32_t creat_data_flow(struct FlowInfo& info);
    static void get_all_data_flow(std::vector<struct FlowInfo> &info);
    static int32_t delete_data_flow(struct FlowInfo &info); // ֻ������ͨ������Ҫ�ֶ�ɾ��
    static int32_t delete_data_flow_server(struct FlowInfo &info); // ֻ�з���˱�����������ģ��ͻ��˱����п���������ֹͣ

private:
    using FlowInfoPortMap = std::map<int32_t, struct FlowInfo>; // ��һ������Ϊport
    using FlowInfoServerMap = std::map<std::string, FlowInfoPortMap>; // ��һ������Ϊserverip
    static FlowInfoServerMap m_flow_info_map;

    static int32_t add_data_flow(struct FlowInfo &info);
};


