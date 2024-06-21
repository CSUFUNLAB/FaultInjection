#include "NodeManager.h"
#include "Log.h"
#include "SshSession.h"

#include <functional>

using namespace std;

vector<struct NodeManager::NodeInfo> NodeManager::m_node_info_list = {
    {
        0,
        string("linux server"),
        string("eth"),
        string("enp0s31f6"),
        string("18:c0:4d:0d:f5:e2"),
        string("192.168.3.82"),
        true,
        nullptr,
    },
    {
        1,
        string("antsdr"),
        string("eth"),
        string("eth0"),
        string("00:0a:35:00:01:22"),
        string("192.168.3.10"),
        true,
        nullptr,
    },
    {
        2,
        string("antsdr"),
        string("ap"),
        string("sdr0"),
        string("66:55:44:33:22:09"),
        string("192.168.13.1"),
        true,
    },
    {
        3,
        string("orangepi"),
        string("sta"),
        string("wlan0"),
        string("f0:23:ae:09:80:bc"),
        string("none"), // ip 未知
        false,
        nullptr,
    },
    {
        4,
        string("orangepi"),
        string("sta"),
        string("wlan0"),
        string("54:78:c9:07:8b:1c"),
        string("none"), // ip 未知
        false,
        nullptr,
    },
    {
        5,
        string("orangepi"),
        string("sta"),
        string("wlan0"),
        string("54:78:c9:07:8a:cc"),
        string("none"), // ip 未知
        false,
        nullptr,
    },
};

string NodeManager::m_error_return = "none";

bool NodeManager::node_num_exist(int32_t node_num)
{
    if (node_num < 0 || node_num > m_node_info_list.size()) {
        LOG_ERR("no node[{}]", node_num);
        return false;
    }
    if (m_node_info_list[node_num].detected == false) {
        return false;
    }
    return true;
}

bool NodeManager::node_ip_exist(std::string &ip)
{
    for (auto& node : m_node_info_list) {
        if (node.ip == ip) {
            return true;
        }
    }
    LOG_ERR("no node[{}]", ip);
    return false;
}

bool NodeManager::node_mac_exist(std::string &mac)
{
    for (auto& node : m_node_info_list) {
        if (node.mac == mac) {
            return true;
        }
    }
    LOG_ERR("no node[{}]", mac);
    return false;
}

string &NodeManager::node_num_to_ip(int32_t node_num)
{
    if (!node_num_exist(node_num)) {
        return m_error_return;
    }
    return m_node_info_list[node_num].ip;
}

int32_t NodeManager::node_mac_to_num(std::string mac)
{
    for (int32_t i = 0; i < m_node_info_list.size(); i++) {
        if (m_node_info_list[i].mac == mac) {
            return i;
        }
    }
    return -1;
}

struct NodeManager::NodeInfo* NodeManager::get_node_info(int32_t node_num)
{
    if (!node_num_exist(node_num)) {
        return nullptr;
    }
    return &m_node_info_list[node_num];
}


class NodeManagerSsh : public SshSession {
public:
    using SshSession::SshSession;

    void read_echo(char* data)  override;
    //void cmd_end(void)  override;

    void get_sta_ip(char* buff);
    int32_t get_sta_ip_read_echo(int32_t cout, char *buff);
    int32_t sta_mac_to_ip(int32_t cout, char *buff);
    int32_t ap_dump_sta_mac(int32_t cout, char *buff);

private:
    int32_t m_cmd_index = 0;
    std::string m_station_ip; // arp -n 的时候会先读ip，记录
    std::vector<std::string> m_station_mac; // station dump的时候会获得多个mac，记录

};

int32_t NodeManagerSsh::sta_mac_to_ip(int32_t cout, char* buff)
{
    if (cout == 0 && isdigit(buff[0]) == 0) { // 只看ip地址开始的行
        return 1;
    }
    if (cout == 0) {
        m_station_ip = buff;
    } else if (cout == 2) {
        string get_mac = buff;
        for (auto& mac : m_station_mac) {
            if (get_mac != mac) {
                continue;
            }
            for (auto& node : NodeManager::m_node_info_list) {
                if (node.mac == get_mac) {
                    node.ip = m_station_ip;
                    node.up_linked = m_node_info;
                    node.detected = true;
                    LOG_INFO("node[{}] is sta of [{}]", m_station_ip, m_node_info->ip);
                }
            }
        }
        return 0;
    }
    return -1;
}

int32_t NodeManagerSsh::ap_dump_sta_mac(int32_t cout, char* buff)
{
    if (cout == 0 && buff[0] != 'S') { // 只看Station开始的行
        return 1;
    }
    if (cout == 1) {
        LOG_DEBUG("ap[{}] sta mac[{}]", m_node_info->ip, buff);
        m_station_mac.push_back(buff);
        return 0;
    }
    return -1;
}

int32_t NodeManagerSsh::get_sta_ip_read_echo(int32_t cout, char* buff)
{
    if (m_cmd_index == 0) {
        return ap_dump_sta_mac(cout, buff);
    } else if (m_cmd_index == 1) {
        return sta_mac_to_ip(cout, buff);
    }
}

void NodeManagerSsh::get_sta_ip(char *buff)
{
    char* space_token = nullptr;
    char* next_space_token = nullptr;
    char* next_line_token = nullptr;
    char* line_token = strtok_s(buff, "\n", &next_line_token);
    int32_t ret = -1;
    int32_t cout; // of space token
    while (line_token != nullptr) {
        LOG_DEBUG("line {}", line_token);
        space_token = strtok_s(line_token, " ", &next_space_token);
        LOG_DEBUG("space {}", space_token);
        cout = 0;
        while (space_token != nullptr) {
            ret = get_sta_ip_read_echo(cout, space_token);
            // > 0 表示跳过这一行，但是未找到结果
            // = 0 表示这一行已经看完了，看下一行
            if (ret >= 0) {
                break;
            }
            cout++;
            space_token = strtok_s(nullptr, " ", &next_space_token);
        }
        line_token = strtok_s(nullptr, "\n", &next_line_token);
    }
    if (ret == 0) {
         // TODO: 如果返回结果每次只有一行导致不能一次性处理完，这里加一个read byte = 0的条件
        if (m_cmd_index == 0) {
            send_cmd("arp -n\n");
            m_cmd_index = 1;
        }
        if (m_cmd_index == 1) {
            LOG_INFO("get all sta of ap[{}]", m_node_info->ip);
            m_last_cmd = true;
        }
    }
}

void NodeManagerSsh::read_echo(char* data)
{
    get_sta_ip(data);
}

//void NodeManagerSsh::cmd_end(void)
//{
//    close();
//    delete this;
//}

int32_t NodeManager::get_sta_ip(NodeInfo &info)
{
    NodeManagerSsh *ssh = new NodeManagerSsh(&info); // 没注册cmd_emd，ssh结束后自动delete

    int32_t ret = ssh->open();
    ERR_RETURN_PRINT(ret != NORMAL_OK, NORMAL_ERR, "open src ssh[{}]", info.ip);

    ssh->m_last_cmd = false;

    ssh->send_cmd(string("iw dev sdr0 station dump | grep Station") + "\n");

    return 0;
}

void NodeManager::get_all_sta_ip(void)
{
    m_node_info_list[0].up_linked = &m_node_info_list[0];
    m_node_info_list[1].up_linked = &m_node_info_list[0];
    m_node_info_list[2].up_linked = &m_node_info_list[1];

    for (auto& node : NodeManager::m_node_info_list) {
        node.detected = false;
    }
    // TODO: 使用ping来检测ap
    m_node_info_list[0].detected = true;
    m_node_info_list[1].detected = true;
    m_node_info_list[2].detected = true;

    int32_t ret;
    for (auto& node : NodeManager::m_node_info_list) {
        if (node.type == "ap") {
            ret = get_sta_ip(node);
            LOG_INFO("scan node[{}][{}]", node.index, ret);
        }
    }
}

