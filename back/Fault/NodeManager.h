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
        bool app_down; // �˽ڵ��޷�����app
        bool cpu_over_loader;
    };
    static std::vector<struct NodeInfo> m_node_info_list;
    // ���Ҫ��ʹ��m_node_info_list֮ǰִ�У������ڹ��캯�������ԭ���ǣ�m_node_info_list��������֮ǰ������
    void init_m_node_info_list(void);
    static NodeManager* get_instance(void);

    bool node_num_exist(int32_t node_num);
    bool node_ip_exist(std::string& ip);
    bool node_mac_exist(std::string& mac);
    uint32_t get_password_loop(struct NodeInfo* node);
    struct NodeInfo* get_node_info(int32_t node_num); // �ڲ����ж�node�Ƿ񱻼�⵽

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

