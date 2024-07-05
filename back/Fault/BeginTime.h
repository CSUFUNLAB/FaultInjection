#pragma once
#include <stdint.h>
#include <iostream>
#include <chrono>
#include <thread>

// 这个时间是从程序运行时间开始计算
class BeginTime {
public:
    BeginTime();
    uint32_t get_time();
    static BeginTime *get_instance();
private:
    std::chrono::time_point<std::chrono::system_clock> start_time;
};
