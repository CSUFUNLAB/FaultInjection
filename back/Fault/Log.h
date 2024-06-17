#pragma once

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/std.h>

#define LOG_DEBUG_ON 0

#define LOG_PRINT(log, ...) fmt::print("[{}][{}]" log "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_ERR(log, ...) LOG_PRINT("[error]" log, ##__VA_ARGS__)
#define LOG_INFO(log, ...) LOG_PRINT("[info]" log, ##__VA_ARGS__)

#if LOG_DEBUG_ON == 1
#define LOG_DEBUG(log, ...) LOG_PRINT("[debug]" log, ##__VA_ARGS__)
#else
#define LOG_DEBUG(log, ...)
#endif

#define ERR_RETURN(condition, err_return, log, ...) if(condition) {LOG_ERR(log, ##__VA_ARGS__); return err_return;}
#define ERR_RETURN_PRINT(condition, err_return, log, ...) \
	if(condition) {LOG_ERR(log, ##__VA_ARGS__); return err_return;} else {LOG_INFO("[success]" log, ##__VA_ARGS__);}

#define NORMAL_OK 0
// 这种错误没什么特别的
#define NORMAL_ERR -1
#define NO_NODE -2
#define EXIST_CMD -3
