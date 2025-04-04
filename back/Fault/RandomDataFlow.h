#pragma once

#include <vector>
#include <stdint.h>

class RandomNum {
public:
    static RandomNum *get_instance(void);
    static bool get_bool_random(void);
    uint32_t range_random_num(uint32_t low, uint32_t high);
};

// 获取已检测节点的随机节点
class RandomNode : public RandomNum {
public:
    static RandomNode *get_instance(void);
    void get_detect_node(void); // 这个函数需要在检测节点后调用
    uint32_t get_random_node(void);

private:
    std::vector<uint32_t> m_node_index; // 被检测到的节点vector
};

class RandomDataFlow {
public:
    static RandomDataFlow *get_instance(void);
    void generate_pair_flow(void);
    void stop_generate_pair_flow(void);

private:
    void generate_pair_flow_thread(void);
    bool m_generate_random_flow = true;
    uint32_t max_band = 800;
    uint32_t high_band = 200;
    uint32_t low_band = 50;
};

void random_flow(void);

