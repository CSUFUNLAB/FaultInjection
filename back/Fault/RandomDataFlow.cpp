#include "RandomDataFlow.h"
#include "NodeManager.h"
#include "DataFlow.h"
#include "Log.h"

#include <iostream>
#include <string>
#include <random>
#include <thread>
#include <chrono>

#include "FaultInjection.h"
#include "DataFile.h"

using namespace std;

class RandomNum {
public:
    static RandomNum *get_instance(void);
    static uint32_t get_bool_random(void);
    virtual bool limit_condition(uint32_t num); // 返回true才表示满足limit条件
    uint32_t limit_random_num(uint32_t lowlimit, uint32_t uplimit);
    uint32_t random_num(uint32_t lowlimit, uint32_t uplimit);
};

uint32_t RandomNum::random_num(uint32_t lowlimit, uint32_t uplimit)
{

    random_device rd;  // 用于生成种子
    mt19937 gen(rd()); // 使用Mersenne Twister引擎
    uniform_int_distribution<> dist(lowlimit, uplimit); // 生成1到100之间的整数

    return dist(gen);
}

bool RandomNum::limit_condition(uint32_t num)
{
    return true;
}

uint32_t RandomNum::limit_random_num(uint32_t lowlimit, uint32_t uplimit)
{
    uint32_t num = random_num(lowlimit, uplimit);
    while (!limit_condition(num)) {
        num = random_num(lowlimit, uplimit);
    }
    return num;
}

RandomNum *RandomNum::get_instance(void)
{
    static RandomNum *p = new RandomNum();
    return p;
}

uint32_t RandomNum::get_bool_random(void)
{
    return RandomNum::get_instance()->limit_random_num(0, 1);
}

// 生成两个不一样的node节点
class RandomNode : public RandomNum {
public:
    uint32_t m_pair_node = -1;
    static uint32_t m_biggest_node_num;
    bool limit_condition(uint32_t num) override;
    void get_pair_node(uint32_t& client_node, uint32_t& server_node);
};

uint32_t RandomNode::m_biggest_node_num = NodeManager::m_node_info_list.size() - 1;

bool RandomNode::limit_condition(uint32_t num)
{
    if (m_pair_node < 0) {
        return true;
    }
    if (m_pair_node == num) {
        return false;
    }
    return true;
}

void RandomNode::get_pair_node(uint32_t& client_node, uint32_t& server_node)
{
    m_pair_node = limit_random_num(0, m_biggest_node_num);
    client_node = m_pair_node;
    server_node = limit_random_num(0, m_biggest_node_num);
    m_pair_node = -1;
}

static uint32_t up_limit_flow = 900;
static uint32_t low_limit_flow = 50;

RandomDataFlow *RandomDataFlow::get_instance(void)
{
    static RandomDataFlow *p = new RandomDataFlow();
    return p;
}

void RandomDataFlow::generate_pair_flow(void)
{
    m_generate_random_flow = true;
    std::thread channel_chread(&RandomDataFlow::generate_pair_flow_thread, this);
    channel_chread.detach();
}

void RandomDataFlow::stop_generate_pair_flow(void)
{
    m_generate_random_flow = false;
}

void RandomDataFlow::generate_pair_flow_thread(void)
{
    NodeManager::NodeInfo* node_src;
    NodeManager::NodeInfo* node_dst;
    uint32_t node_src_num;
    uint32_t node_dst_num;
    RandomNode random_node = RandomNode();
    uint32_t send_time;
    uint32_t band_width;
    string type_str;
    string band_width_str;

    while (m_generate_random_flow) {
        random_node.get_pair_node(node_src_num, node_dst_num);
        node_src = NodeManager::get_node_info(node_src_num);
        node_dst = NodeManager::get_node_info(node_dst_num);
        if (node_src == nullptr || node_dst == nullptr) {
            continue;
        }
        // 如果向ap节点发消息，必须发到对应通道的网卡上面，有线发eth0，无线发ap，否则数据会被另一个网卡截留
        // 或许有转发办法
        // eth和ap不能相互通讯，无论是ap到自身eth还是服务器
        if ((node_src->type == "eth" && node_dst->type == "ap")
            || (node_src->type == "ap" && node_dst->type == "eth")) {
            continue;
        }
        // sta不能向ap的eth发消息
        if ((node_src->type == "sta" && node_dst->dev == "eth0")
            || (node_src->dev == "eth0" && node_dst->type == "sta")) {
            continue;
        }
        band_width = RandomNum::get_instance()->limit_random_num(low_limit_flow, up_limit_flow / 5);
        if (node_src->output_band + band_width > up_limit_flow || node_dst->input_band + band_width > up_limit_flow) {
            LOG_INFO("node acc band:[{}:{}][{}:{}]", node_src_num, node_src->output_band, node_dst_num, node_dst->input_band);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        band_width_str = to_string(band_width) + string("K");
        send_time = RandomNum::get_instance()->limit_random_num(10, 30);
        if (RandomNum::get_bool_random() == 1) {
            type_str = "tcp";
        }
        else {
            type_str = "udp";
        }
        DataFlow::FlowInfo flow_info = {
            node_src,
            node_dst,
            band_width_str,
            0,
            send_time,
            type_str,
            nullptr,
            nullptr,
        };
        DataFlow::get_instance()->creat_data_flow(flow_info);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    LOG_INFO("end");
}

void random_fault(void)
{
    int32_t cout = 0;
    int32_t fault_node;
    string cout_string;
    string file_dir;
    AppDown app_down = AppDown(nullptr, cout_string); // 第二个参数没用到

    SshSession remote_ssh(&NodeManager::m_node_info_list[0]);
    remote_ssh.m_send_type = SshSession::QUEUE_CMD;
    remote_ssh.open();

    while (cout++ < 20) {
        LOG_INFO("test[{}] begin", cout);
        NodeManager::get_all_sta_ip();
        cout_string = to_string(cout);
        file_dir = string("~/fault_data/app_down_") + cout_string;
        remote_ssh.only_send_cmd(string("mkdir ") + file_dir + string("\n"));
        DataFile::m_file_dir = file_dir + string("/");

        RandomDataFlow::get_instance()->generate_pair_flow();
        std::this_thread::sleep_for(std::chrono::seconds(60));
        //std::this_thread::sleep_for(std::chrono::seconds(30));
        fault_node = RandomNum::get_instance()->limit_random_num(0, RandomNode::m_biggest_node_num);
        app_down.m_para = to_string(fault_node);
        app_down.fault_injection();
        std::this_thread::sleep_for(std::chrono::seconds(60));
        //std::this_thread::sleep_for(std::chrono::seconds(30));

        RandomDataFlow::get_instance()->stop_generate_pair_flow();
        std::this_thread::sleep_for(std::chrono::seconds(10));
        DataFlow::get_instance()->close_all_data_flow();
        app_down.recover_injection();
        std::this_thread::sleep_for(std::chrono::seconds(5));
        // 不明白为什么仍然有没被删除的flow，理论上close是用broken删除的，只要结束了就一定可以删除
        DataFlow::get_instance()->detect_all_data_flow();
    }
}


