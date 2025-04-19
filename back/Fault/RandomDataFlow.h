#pragma once

#include <vector>
#include <stdint.h>

class RandomNum {
public:
    static RandomNum *get_instance(void);
    static bool get_bool_random(void);
    uint32_t range_random_num(uint32_t low, uint32_t high);
};

// ��ȡ�Ѽ��ڵ������ڵ�
class RandomNode : public RandomNum {
public:
    static RandomNode *get_instance(void);
    void get_detect_node(void); // ���������Ҫ�ڼ��ڵ�����
    uint32_t get_random_node(void);
    std::vector<uint32_t> m_node_index; // ����⵽�Ľڵ�vector
};

class RandomDataFlow {
public:
    static RandomDataFlow *get_instance(void);
    void generate_pair_flow(void);
    void stop_generate_pair_flow(void);

private:
    void generate_pair_flow_thread(void);
    bool m_generate_random_flow = true;
    uint32_t max_band = 1000;
    uint32_t high_band = 400;
    uint32_t low_band = 100;
    uint32_t max_time = 30;
    uint32_t min_time = 10;
};

void random_flow(void);

