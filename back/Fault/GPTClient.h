#pragma once
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <string>
#include <vector>
#include <pplx/pplxtasks.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class GPTClient {
public:
    // 获取唯一实例的静态方法
    static GPTClient& getInstance(const std::string& apiKey = "");

    // 公共方法
    pplx::task<json::value> sendMessage(std::vector<json::value>& messages);

private:
    // 私有构造函数
    GPTClient(const std::string& apiKey);
    // 私有析构函数
    ~GPTClient() = default;
    // 删除复制构造函数
    GPTClient(const GPTClient&) = delete;
    // 删除赋值运算符
    GPTClient& operator=(const GPTClient&) = delete;

    std::string apiKey;
    uri apiUrl;
};
