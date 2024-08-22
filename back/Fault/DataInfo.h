#pragma once
#include <stdint.h>
#include <string>

#include "NodeManager.h"

class DataInfo {
public:
    DataInfo(uint32_t self_node, uint32_t pair_node);

    virtual void upload_info(void);
    int32_t get_info(int32_t index, char* token);
	void deal_iperf_echo(char *buffer);
	void deal_iperf_line_echo(char *buffer);

    using GetDataInfo = int32_t(*)(const char* buff, void* info);

    struct FlowId {
        uint32_t client_node_num;
        uint32_t server_node_num;
        uint32_t time;
        uint32_t port;
        uint32_t *use_cout;
    };

    struct IperfInfo {
        uint32_t self_node;
        uint32_t pair_node; // if is client, it is server node, otherwise
        uint32_t port;
        uint32_t is_client; // 1 is client, 0 is server
        uint32_t trans_type; // 0: tcp; 1: udp
        uint32_t sec;
        float transfer;
        uint32_t transfer_unit; // 1 K M G
        float band;
        uint32_t band_unit; // 1 K M G
        // only tcp client
        uint32_t err;
        uint32_t rtry; // retry
        uint32_t rtt; // Round-Trip Time
        // only udp server
        float lost; // unit %
    };

    struct InfoStrTransfer {
        int32_t index;
        GetDataInfo func;
    };

    struct NodeManager::NodeInfo *m_node = nullptr;
    int32_t m_info_index = 0;
    struct InfoStrTransfer *m_transfer_str = nullptr;
    int32_t m_info_num = 0;
    struct IperfInfo m_iperf_info = { 0 };
    void* m_info_point_arry[8]; // 不同类型消息大小不一样，按照最大的设计
    uint32_t m_begin_time = 0;
};

class TcpClientDataInfo : public DataInfo {
public:
    TcpClientDataInfo(DataInfo::FlowId &flow_id);
};

class TcpServerDataInfo : public DataInfo {
public:
    TcpServerDataInfo(DataInfo::FlowId &flow_id);
};

class UdpClientDataInfo : public DataInfo {
public:
    UdpClientDataInfo(DataInfo::FlowId &flow_id);
};

class UdpServerDataInfo : public DataInfo {
public:
    UdpServerDataInfo(DataInfo::FlowId &flow_id);
};

