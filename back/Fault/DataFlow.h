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
    struct FlowId {
        uint32_t client_node_num;
        uint32_t server_node_num;
        uint32_t time;
        uint32_t port;
        uint32_t *use_cout;
    };
    DataFlowSsh(FlowId &flow_id, std::string &type, NodeManager::NodeInfo *node);
    DataInfo* m_data_info = nullptr;
    struct FlowId m_flow_id;
    void cmd_end(void) override;
    void read_echo(char* data)  override;
};

// 每个dateflow配两个datainfo和两个sshsession
// cmd结束时自动删除datainfo和sshsession，清除记录的dataflow。
class DataFlow {
public:
    struct FlowInfo {
        struct NodeManager::NodeInfo *client; // 自动结束
        struct NodeManager::NodeInfo *server; // 自动结束
        std::string band; // 带单位 K M G
        uint32_t port; // port 自动选择 输入填0即可
        uint32_t time; // s
        std::string type; // tcp or udp 小写
        DataFlowSsh* client_ssh;
        DataFlowSsh* server_ssh;
        uint32_t use_cout; // 用于结束后删除表中存储的此条信息
    };

    static int32_t creat_data_flow(struct FlowInfo& info);
    static void get_all_data_flow(std::vector<struct FlowInfo> &info);
    static void close_all_data_flow(void);
    static int32_t close_data_flow_server(int32_t server_node_num); // 只有服务端崩溃是有意义的，客户端崩溃有可能是主动停止
    static int32_t delete_data_flow(DataFlowSsh::FlowId &flow_id); // 先结束消息才能调用这个接口
    static uint32_t band_width_str_to_num(std::string band_witdh_str);

private:
    using FlowInfoPortMap = std::map<int32_t, struct FlowInfo>; // 第一个参数为port
    using FlowInfoServerMap = std::map<int32_t, FlowInfoPortMap>; // 第一个参数为server node num
    static FlowInfoServerMap m_flow_info_map;
    static struct FlowInfo *add_data_flow(struct FlowInfo &info);
};


