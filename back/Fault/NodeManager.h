#pragma once
#include <string>
#include <vector>

class NodeManager {
public:
    struct NodeInfo {
        const int32_t index;
        const std::string describe;
        const std::string type;
        const std::string dev;
        const std::string mac;
        std::string ip;
        bool detected;
        struct NodeInfo* up_linked;
        int32_t input_band;
        int32_t output_band;
        bool server_fault;
    };
    static std::vector<struct NodeInfo> m_node_info_list;
    static NodeManager* get_instance(void);

    bool node_num_exist(int32_t node_num);
    bool node_ip_exist(std::string& ip);
    bool node_mac_exist(std::string& mac);
    struct NodeInfo* get_node_info(int32_t node_num); // 内部会判断node是否被检测到

    std::string& node_num_to_ip(int32_t node_num);
    int32_t node_mac_to_num(std::string mac);
    void get_all_sta_ip(void);

private:
    static std::string m_error_return;
    void get_sta_ip(NodeInfo& info);
    void get_adhoc_ip(NodeInfo& info);
};

