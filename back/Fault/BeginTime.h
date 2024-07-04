#pragma once
#include <stdint.h>
#include <iostream>
#include <chrono>
#include <thread>

// ���ʱ���Ǵӳ�������ʱ�俪ʼ����
class BeginTime {
public:
    BeginTime();
    uint32_t get_time();
    static BeginTime *get_instance();
private:
    std::chrono::time_point<std::chrono::system_clock> start_time;
};
