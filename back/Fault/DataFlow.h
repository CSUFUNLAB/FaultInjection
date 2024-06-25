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

// 每个dateflow配两个datainfo和两个sshsession
// cmd结束时自动删除datainfo和sshsession，清除记录的dataflow。
class DataFlow {
public:
    struct FlowInfo {
        struct NodeManager::NodeInfo *client;
        struct NodeManager::NodeInfo *server;
        std::string bind; // 带单位 K M G
        uint32_t port; // port 自动选择 输入填0即可
        uint32_t time; // s
        std::string type; // tcp or udp 小写
        DataFlowSsh* client_ssh;
        DataFlowSsh* server_ssh;
    };

    static int32_t creat_data_flow(struct FlowInfo& info);
    static void get_all_data_flow(std::vector<struct FlowInfo> &info);
    static int32_t delete_data_flow(struct FlowInfo &info); // 只有永久通道才需要手动删除
    static int32_t delete_data_flow_server(struct FlowInfo &info); // 只有服务端崩溃是有意义的，客户端崩溃有可能是主动停止

private:
    using FlowInfoPortMap = std::map<int32_t, struct FlowInfo>; // 第一个参数为port
    using FlowInfoServerMap = std::map<std::string, FlowInfoPortMap>; // 第一个参数为serverip
    static FlowInfoServerMap m_flow_info_map;

    static int32_t add_data_flow(struct FlowInfo &info);
};


