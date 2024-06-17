#pragma once
#include <stdint.h>
#include <string>

#include "NodeManager.h"

class DataInfo {
public:
    virtual void upload_info(void) = 0;
    int32_t get_info(int32_t index, char* token);
	void deal_iperf_echo(char *buffer);
	void deal_iperf_line_echo(char *buffer);

    using GetDataInfo = int32_t(*)(const char* buff, void* info);

    struct IperfInfo {
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
    void* *m_info_point = nullptr;
};

class TcpClientDataInfo : public DataInfo {
public:
    TcpClientDataInfo(struct NodeManager::NodeInfo *node);
    void upload_info(void) override;
    void* m_info_point_arry[8];
};

class TcpServerDataInfo : public DataInfo {
public:
    TcpServerDataInfo(struct NodeManager::NodeInfo *node);
    void upload_info(void) override;
    void* m_info_point_arry[5];
};

class UdpClientDataInfo : public DataInfo {
public:
    UdpClientDataInfo(struct NodeManager::NodeInfo *node);
    void upload_info(void) override;
    void* m_info_point_arry[5];
};

class UdpServerDataInfo : public DataInfo {
public:
    UdpServerDataInfo(struct NodeManager::NodeInfo *node);
    void upload_info(void) override;
    void* m_info_point_arry[6];
};

