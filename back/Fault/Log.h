#pragma once

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/std.h>

#include <iostream>
#include <fstream>

#include "BeginTime.h"

std::ofstream &logfile(void);

#define LOG_DEBUG_ON 0

#define LOG_PRINT(log, ...) fmt::print("[{}][{}][{}s]" log "\n", __FUNCTION__, __LINE__, BeginTime::get_instance()->get_time(), ##__VA_ARGS__)
//#define LOG_PRINT(log, ...) { logfile() << fmt::format("[{}][{}][{}s]" log, __FUNCTION__, __LINE__, BeginTime::get_instance()->get_time(), ##__VA_ARGS__) << std::endl; }
#define LOG_ERR(log, ...) LOG_PRINT("[error]" log, ##__VA_ARGS__)
#define LOG_INFO(log, ...) LOG_PRINT("[info]" log, ##__VA_ARGS__)

#if LOG_DEBUG_ON == 1
#define LOG_DEBUG(log, ...) LOG_PRINT("[debug]" log, ##__VA_ARGS__)
#else
#define LOG_DEBUG(log, ...)
#endif

#define ERR_RETURN(condition, err_return, log, ...) if(condition) {LOG_ERR(log, ##__VA_ARGS__); return err_return;}

#define ERR_RETURN_DEBUG_PRINT(condition, err_return, log, ...) \
	if(condition) {LOG_ERR(log, ##__VA_ARGS__); return err_return;} else {LOG_DEBUG("[success]" log, ##__VA_ARGS__);}

#define ERR_RETURN_PRINT(condition, err_return, log, ...) \
	if(condition) {LOG_ERR(log, ##__VA_ARGS__); return err_return;} else {LOG_INFO("[success]" log, ##__VA_ARGS__);}

enum FAULT_INJECT_ERR_CODE {
    NORMAL_OK = 0 ,
    NORMAL_ERR, // 一般错误
    DEFINE_ERR, // 自定义错误，手动写入m_handler_info.msg
    NO_NODE, // 不存在此节点
    NO_EXIST_FLOW, // 不存在此数据流
    NO_OPERATE, // 不存在次操作
    SSH_ERR, // ssh错误
    ERR_CODE_BUTT, // 不存在的错误码
};
