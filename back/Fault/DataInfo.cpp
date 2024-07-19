#include "DataInfo.h"
#include "DataFile.h"
#include "LOG.h"

#include <cstring>
#include <string>

using namespace std;
using NodeInfo = struct NodeManager::NodeInfo;

static int32_t get_int_num(const char* buff, void* info)
{
    int32_t num = atoi(buff);
    memcpy(info, &num, sizeof(num));
    return 0;
}
static int32_t get_in_brackets(const char* buff, void* info)
{
    LOG_DEBUG("lost is {}", buff);
    if (buff[0] != '(') {
        return -1;
    }
    const char* p = strchr(buff, '%');
    int32_t len = p - buff;
    if (len > 10 || len < 1) {
        return 0;
    }
    char temp[16];
    memcpy(temp, buff + 1, len);
    temp[len - 1] = '\0';
    float num = atof(temp);
    memcpy(info, &num, sizeof(num));
    return 0;
}

static int32_t get_float_num(const char* buff, void* info)
{
    float num = atof(buff);
    memcpy(info, &num, sizeof(num));
    return 0;
}

static int32_t get_time_index(const char* buff, void* info)
{
    LOG_DEBUG("time index {}", buff);
    int32_t num = atoi(buff);
    memcpy(info, &num, sizeof(num));
    return 0;
}

static int32_t get_unit(const char* buff, void* info)
{
    int32_t unit = 1;
    if (buff[0] == 'M') {
        unit = 1000000;
    } else if (buff[0] == 'K') {
        unit = 1000;
    } else if (buff[0] == 'G') {
        unit = 1000000000;
    }
    memcpy(info, &unit, sizeof(unit));
    return 0;
}

// num_a/num_b, get num b
static int32_t get_slash_right_num(const char* buff, void* info)
{
    const char* p = strchr(buff, '/');
    if (p == nullptr) {
        return 0;
    }

    int32_t num = atoi(p + 1);
    memcpy(info, &num, sizeof(num));
    return 0;
}

struct DataInfo::InfoStrTransfer tcp_client_transfer_str[] = {
    {
        2, // sec
        get_time_index,
    },
    {
        4, // transfer
        get_float_num,
    },
    {
        5, // transfer unit
        get_unit,
    },
    {
        6, // bandwith
        get_float_num,
    },
    {
        7, // band_unit
        get_unit,
    },
    {
        8, // write/err
        get_slash_right_num,
    },
    {
        9, // infory
        get_int_num,
    },
    {
        10, // cwnd/rtt
        get_slash_right_num,
    },
};

struct DataInfo::InfoStrTransfer tcp_server_transfer_str[] = {
    {
        2, // sec
        get_time_index,
    },
    {
        4, // transfer
        get_float_num,
    },
    {
        5, // transfer unit
        get_unit,
    },
    {
        6, // bandwith
        get_float_num,
    },
    {
        7, // band_unit
        get_unit,
    },
};

struct DataInfo::InfoStrTransfer udp_client_transfer_str[] = {
    {
        2, // sec
        get_time_index,
    },
    {
        4, // transfer
        get_float_num,
    },
    {
        5, // transfer unit
        get_unit,
    },
    {
        6, // bandwith
        get_float_num,
    },
    {
        7, // band_unit
        get_unit,
    },
    {
        11, // lost
        get_in_brackets,
    },
    { // 前面的数字A/B可能是A/ B，按照空格分割会多一个
        12, // lost
        get_in_brackets,
    },
};

int32_t DataInfo::get_info(int32_t index, char* token)
{
    int32_t i;
    if (m_info_index >= m_info_num) {
        return 0; // index已经找完了不必继续了
    }
    for (i = 0; i < m_info_num; i++) {
        if (m_transfer_str[i].index == index) {
            if (m_transfer_str[i].func(token, m_info_point_arry[m_info_index]) == 0) {
                m_info_index++;
            }
            return 1;
        }
    }
    return 1;
}

void DataInfo::upload_info(void)
{
    string arrow;
    string tcp;
#if 0
    if (m_iperf_info.is_client) {
        arrow = " -> ";
    } else {
        arrow = " <- ";
    }
    if (m_iperf_info.trans_type == 0) {
        tcp = "tcp";
    } else {
        tcp = "udp";
    }
#endif
    LOG_DEBUG("[{}] {}" + arrow + "{} {} time[{}] transfer[{}*{}] band[{}*{}] err[{}] retry[{}] rtt[{}] lost[{}%]",
        m_node->ip,
        m_iperf_info.self_node,
        m_iperf_info.pair_node,
        tcp,
        m_iperf_info.sec,
        m_iperf_info.transfer,
        m_iperf_info.transfer_unit,
        m_iperf_info.band,
        m_iperf_info.band_unit,
        m_iperf_info.err,
        m_iperf_info.rtry,
        m_iperf_info.rtt,
        m_iperf_info.lost
    );
    m_iperf_info.sec += m_begin_time;
    DataFile::get_instance()->send_data_info(m_iperf_info);
}

DataInfo::DataInfo(NodeInfo *self_node, NodeInfo *pair_node)
{
    m_node = self_node;
    m_iperf_info.self_node = m_node->index;
    m_iperf_info.pair_node = pair_node->index;

    m_info_point_arry[0] = &m_iperf_info.sec;
    m_info_point_arry[1] = &m_iperf_info.transfer;
    m_info_point_arry[2] = &m_iperf_info.transfer_unit;
    m_info_point_arry[3] = &m_iperf_info.band;
    m_info_point_arry[4] = &m_iperf_info.band_unit;
}

TcpClientDataInfo::TcpClientDataInfo(NodeInfo *self_node, NodeInfo *pair_node) : DataInfo(self_node, pair_node)
{
    m_iperf_info.is_client = 1;
    m_iperf_info.trans_type = 0;

    m_transfer_str = tcp_client_transfer_str;
    m_info_point_arry[5] = &m_iperf_info.err;
    m_info_point_arry[6] = &m_iperf_info.rtry;
    m_info_point_arry[7] = &m_iperf_info.rtt;
    m_info_num = 8;
}

TcpServerDataInfo::TcpServerDataInfo(NodeInfo *self_node, NodeInfo *pair_node) : DataInfo(self_node, pair_node)
{
    m_iperf_info.is_client = 0;
    m_iperf_info.trans_type = 0;

    m_transfer_str = tcp_server_transfer_str;
    m_info_num = 5;
}

UdpClientDataInfo::UdpClientDataInfo(NodeInfo *self_node, NodeInfo *pair_node) : DataInfo(self_node, pair_node)
{
    m_iperf_info.is_client = 1;
    m_iperf_info.trans_type = 1;

    m_transfer_str = tcp_server_transfer_str; // 二者一致
    m_info_num = 5;
}

UdpServerDataInfo::UdpServerDataInfo(NodeInfo *self_node, NodeInfo *pair_node) : DataInfo(self_node, pair_node)
{
    m_iperf_info.is_client = 0;
    m_iperf_info.trans_type = 1;

    m_transfer_str = udp_client_transfer_str;
    m_info_point_arry[5] = &m_iperf_info.lost;
    m_info_num = 7; // lost 会找两次
}

void DataInfo::deal_iperf_line_echo(char* buffer)
{
    int32_t cout = 0;
    char* next_token = nullptr;
    // 第一个是[
    char* token = strtok_s(buffer, " ", &next_token);
    if (token == nullptr || token[0] != '[') {
        return;
    }

    // 第二个是 x]
    cout++;
    token = strtok_s(nullptr, " ", &next_token);
    if (token == nullptr || token[1] != ']') {
        return;
    }

    // 第三个是数字开始
    cout++;
    token = strtok_s(nullptr, " ", &next_token);
    if (token == nullptr || isdigit(token[0]) == 0) {
        return;
    }
    // 在iperf结束的时候 client 会有一个 0 - end 的总结，不计入统计
    // server 会有一个大于传输时间一点的传输，不计入统计
    int32_t begin_time = atoi(token);
    int32_t end_time = 0;
    const char* p = strchr(token, '-');
    if (p != nullptr) {
        end_time = atoi(p + 1);
    }
    if (end_time == 0
        || ((begin_time == 0 && end_time != 1)
        || begin_time == end_time)) {
        return;
    }
    while (token != nullptr) {
        LOG_DEBUG("[{}][{}][{}]", m_node->ip, cout, token);
        if (get_info(cout, token) == 0) {
            break;
        }
        cout++;
        token = strtok_s(nullptr, " ", &next_token);
    }
    upload_info();
}

void DataInfo::deal_iperf_echo(char* buffer)
{
    char* next_token = nullptr;
    char* token = strtok_s(buffer, "\n", &next_token);
    while (token != nullptr) {
        //if (m_ip == "192.168.13.5") {
        //    LOG_DEBUG("[{}]{}", m_node->ip, token);
        //}
        deal_iperf_line_echo(token);
        token = strtok_s(nullptr, "\n", &next_token);
        m_info_index = 0;
    }
}


