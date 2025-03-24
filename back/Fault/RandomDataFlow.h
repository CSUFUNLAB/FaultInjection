#pragma once

#include <vector>
#include <stdint.h>

class RandomNum {
public:
    static RandomNum *get_instance(void);
    static bool get_bool_random(void);
    virtual bool limit_condition(uint32_t num); // ����true�ű�ʾ����limit����
    uint32_t limit_random_num(uint32_t low, uint32_t high); // �������������Ҫͨ��limit condtion
    uint32_t range_random_num(uint32_t low, uint32_t high);
};

class RandomNode : public RandomNum {
public:
    static RandomNode *get_instance(void);
    void get_detect_node(void); // ���������Ҫ�ڼ��ڵ�����
    uint32_t get_random_node(void);

private:
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
    uint32_t max_band = 900;
    uint32_t high_band = 200;
    uint32_t low_band = 50;
};

void random_flow(void);

