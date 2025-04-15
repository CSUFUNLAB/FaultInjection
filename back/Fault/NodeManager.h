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
        uint32_t need_password;
        bool detected;
        struct NodeInfo* up_linked;
        bool app_down; // 此节点无法创建app
        bool cpu_over_loader;
    };
    static std::vector<struct NodeInfo> m_node_info_list;
    // 这个要在使用m_node_info_list之前执行，不放在构造函数里面的原因是，m_node_info_list可以在这之前被调用
    void init_m_node_info_list(void);
    static NodeManager* get_instance(void);

    bool node_num_exist(int32_t node_num);
    bool node_ip_exist(std::string& ip);
    bool node_mac_exist(std::string& mac);
    uint32_t get_password_loop(struct NodeInfo* node);
    struct NodeInfo* get_node_info(int32_t node_num); // 内部会判断node是否被检测到

    using node_deal_func = void(*)(struct NodeInfo* node);
    void get_detected_node(node_deal_func func);

    std::string& node_num_to_ip(int32_t node_num);
    int32_t node_mac_to_num(std::string mac);
    void get_all_sta_ip(void);

private:
    static std::string m_error_return;
    void get_sta_ip(NodeInfo& info);
    void get_adhoc_ip(NodeInfo& info);
};

