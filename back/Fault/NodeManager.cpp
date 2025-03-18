#include "NodeManager.h"
#include "Log.h"
#include "SshSession.h"

#include <atomic>
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
        1,
        false,
        nullptr,
    },
    {
        1,
        string("antsdr"),
        string("eth"),
        string("eth0"),
        string("00:0a:35:00:01:22"),
        string("192.168.3.10"),
        1,
        false,
        nullptr,
    },
    {
        2,
        string("antsdr"),
        string("abandon"),
        string("sdr0"),
        string("66:55:44:33:22:09"),
        string("192.168.13.1"),
        1,
        false,
        nullptr,
    },
    {
        3,
        string("orangepi"),
        string("ap"),
        string("ap0"),
        string("f0:23:ae:09:80:bc"),
        string("192.168.12.1"),
        0,
        false,
        nullptr,
    },
    {
        4,
        string("orangepi"),
        string("sta"),
        string("wlan0"),
        string("54:78:c9:07:8b:1c"),
        string("0.0.0.0"),
        1,
        false,
        nullptr,
    },
    {
        5,
        string("orangepi"),
        string("sta"),
        string("wlan0"),
        string("54:78:c9:07:8a:cc"),
        string("0.0.0.0"),
        1,
        false,
        nullptr,
    },
    {
        6,
        string("orangepi"),
        string("adhoc_master"), // ap本身没有master的属性，这个ap表示是连接服务器的ap
        string("wlan0"),
        string("9c:b8:b4:5d:e8:54"),
        string("192.168.13.1"),
        1,
        false,
        nullptr,
    },
    {
        7,
        string("orangepi"),
        string("adhoc"),
        string("wlan0"),
        string("9c:b8:b4:5d:f6:9a"),
        string("0.0.0.0"),
        1,
        false,
        nullptr,
    },
    {
        8,
        string("orangepi"),
        string("adhoc"),
        string("wlan0"),
        string("9c:b8:b4:5d:f7:98"),
        string("0.0.0.0"),
        1,
        false,
        nullptr,
    },
    {
        9,
        string("orangepi"),
        string("adhoc"),
        string("wlan0"),
        string("9c:b8:b4:5d:f7:58"),
        string("0.0.0.0"),
        1,
        false,
        nullptr,
    },
    {
        10,
        string("orangepi"),
        string("sta"),
        string("wlan0"),
        string("9c:b8:b4:5d:f7:6c"),
        string("192.168.12.167"),
        1,
        false,
        nullptr,
    },
    {
        11,
        string("orangepi"),
        string("sta"),
        string("wlan0"),
        string("9c:b8:b4:5d:f7:6e"),
        string("0.0.0.0"),
        1,
        false,
        nullptr,
    },
    {
        12,
        string("orangepi"),
        string("sta"),
        string("wlan0"),
        string("9c:b8:b4:5d:f6:7c"),
        string("0.0.0.0"),
        1,
        false,
        nullptr,
    },
    {
        13,
        string("orangepi"),
        string("sta"),
        string("wlan0"),
        string("9c:b8:b4:5d:e6:fa"),
        string("0.0.0.0"),
        1,
        false,
        nullptr,
    },
    {
        14,
        string("orangepi"),
        string("sta"),
        string("wlan0"),
        string("9c:b8:b4:5d:e8:44"),
        string("0.0.0.0"),
        1,
        false,
        nullptr,
    },
};

string NodeManager::m_error_return = "none";

NodeManager* NodeManager::get_instance(void)
{
    static NodeManager* p = new NodeManager();
    return p;
}

bool NodeManager::node_num_exist(int32_t node_num)
{
    if (node_num < 0 || node_num >= m_node_info_list.size()) {
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
    for (auto &node : m_node_info_list) {
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

uint32_t NodeManager::get_password_loop(struct NodeInfo* node)
{
    if (node == nullptr) {
        return 0;
    }
    return node->need_password + get_password_loop(node->up_linked);
}

void NodeManager::get_detected_node(node_deal_func func)
{
    for (auto& node : NodeManager::m_node_info_list) {
        if (node.detected) {
            func(&node);
        }
    }
}

class NodeManagerSsh : public SshSession {
public:
    using SshSession::SshSession;
    void read_echo(char* data) override;
    void cmd_end(void) override;
    virtual int32_t data_deal(int32_t cout, char* buff) = 0;
};

void NodeManagerSsh::cmd_end(void)
{
    // ssh关的快，不能自动删除自己
    close();
}

void NodeManagerSsh::read_echo(char* buff)
{
    char* space_token = nullptr;
    char* next_space_token = nullptr;
    char* next_line_token = nullptr;
    char* line_token = strtok_s(buff, "\n", &next_line_token);
    int32_t ret = -1;
    int32_t cout; // of space token
    while (line_token != nullptr) {
        // LOG_DEBUG("line {}", line_token);
        space_token = strtok_s(line_token, " ", &next_space_token);
        // LOG_DEBUG("space {}", space_token);
        cout = 0;
        while (space_token != nullptr) {
            ret = data_deal(cout, space_token);
            // > 0 表示跳过这一行，但是未找到结果
            // = 0 表示这一行已经看完了，看下一行
            // < 0 则看下一个空格
            if (ret >= 0) {
                break;
            }
            cout++;
            space_token = strtok_s(nullptr, " ", &next_space_token);
        }
        line_token = strtok_s(nullptr, "\n", &next_line_token);
    }
    LOG_DEBUG("get adhoc ip end");
}

class NodeManagerArpSsh : public NodeManagerSsh {
public:
    using NodeManagerSsh::NodeManagerSsh;
    int32_t data_deal(int32_t cout, char* buff);
    vector<string> m_arp_ip;
    vector<string> m_arp_mac;
};

int32_t NodeManagerArpSsh::data_deal(int32_t cout, char* buff)
{
    if (cout == 0 && isdigit(buff[0]) == 0) { // 只看ip地址开始的行
        return 1;
    }
    if (cout == 0) {
        LOG_INFO("get ip {}", buff);
        m_arp_ip.push_back(buff);
    } else if (cout == 2) {
        LOG_INFO("get mac {}", buff);
        m_arp_mac.push_back(buff);
        return 0;
    }
    return -1;
}

class NodeManagerStaDumpSsh : public NodeManagerSsh {
public:
    using NodeManagerSsh::NodeManagerSsh;
    int32_t data_deal(int32_t cout, char* buff);
    vector<string> m_sta_mac;
};

int32_t NodeManagerStaDumpSsh::data_deal(int32_t cout, char* buff)
{
#if 0
    // 不知道为什么，buff直接打印是Station，但是buff[i]一个个打印却包含异常字符
    bool is_Station = false;
    for (int i = 0; i < strlen(buff); i++) {
        if (buff[i] == 'S') {
            if (buff[i + 1] == 't') {
                is_Station = true;
                break;
            }
        }
    }
#endif
    if (cout == 0 && buff[0] == 'S') { // 只看Station开始的行
        LOG_INFO("read next space");
        return -1;
    }
    if (cout == 1) {
        LOG_INFO("ap[{}] sta mac[{}]", m_node_info->ip, buff);
        m_sta_mac.push_back(buff);
        return 0;
    }
    LOG_DEBUG("not Station begin line");
    return 1;
}


void NodeManager::get_sta_ip(NodeInfo &info)
{
    NodeManagerStaDumpSsh *sta_ssh = new NodeManagerStaDumpSsh(&info);

    string cmd_line = "iw dev " + info.dev + " station dump | grep Station";
    sta_ssh->m_send_type = SshSession::SHELL_CMD;
    sta_ssh->python_ssh(cmd_line);
    while (sta_ssh->m_send_cmd) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    NodeManagerArpSsh *arp_ssh = new NodeManagerArpSsh(&info);
    arp_ssh->m_send_type = SshSession::SHELL_CMD;
    arp_ssh->python_ssh("arp -n");
    while (arp_ssh->m_send_cmd) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    for (auto& mac : sta_ssh->m_sta_mac) {
        for (auto& node : NodeManager::m_node_info_list) {
            if (node.mac != mac) {
                continue;
            }
            LOG_INFO("set node[{}] ip", node.index);
            for (int i = 0; i < arp_ssh->m_arp_mac.size(); i++) {
                if (mac != arp_ssh->m_arp_mac[i]) {
                    continue;
                }
                node.ip = arp_ssh->m_arp_ip[i];
                node.up_linked = &info;
                node.detected = true;
                LOG_INFO("node[{}] is sta of [{}]", node.ip, info.ip);
                break;
            }
            break;
        }
    }
    delete sta_ssh;
    delete arp_ssh;
}

void NodeManager::get_adhoc_ip(NodeInfo& info)
{
    NodeManagerArpSsh *arp_ssh = new NodeManagerArpSsh(&info);
    arp_ssh->m_send_type = SshSession::SHELL_CMD;
    arp_ssh->python_ssh("arp -n");
    while (arp_ssh->m_send_cmd) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    for (auto& node : NodeManager::m_node_info_list) {
        for (int i = 0; i < arp_ssh->m_arp_mac.size(); i++) {
            if (node.mac != arp_ssh->m_arp_mac[i]) {
                continue;
            }
            node.ip = arp_ssh->m_arp_ip[i];
            node.up_linked = &info;
            node.detected = true;
            LOG_INFO("node[{}] is sta of [{}]", node.ip, info.ip);
            break;
        }
        break;
    }
    delete arp_ssh;
}

void NodeManager::get_all_sta_ip(void)
{
    m_node_info_list[0].up_linked = &m_node_info_list[0];
    m_node_info_list[1].up_linked = &m_node_info_list[0];
    m_node_info_list[2].up_linked = &m_node_info_list[1];

#if 0
    // adhoc检测
    m_node_info_list[6].detected = true;
    for (auto& node : NodeManager::m_node_info_list) {
        if (node.type == "adhoc_master") {
            get_adhoc_ip(node);
            break;
        }
    }
#endif
#if 1
    // sta检测
    m_node_info_list[3].detected = true;
    for (auto& node : NodeManager::m_node_info_list) {
        if (node.type == "ap") {
            get_sta_ip(node);
            LOG_INFO("scan node[{}]", node.index);
        }
    }
#endif
}

