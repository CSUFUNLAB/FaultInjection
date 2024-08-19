#pragma once
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <string>
#include <vector>
#include <pplx/pplxtasks.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;
class Message;
class GPTClient {
public:
    // 获取唯一实例的静态方法
    static GPTClient& get_instance();

    // 加载配置参数
    void load_config();

    // 清空聊天记录
    void reset_messages();

    // 遍历打印messages
    void print_messages() const;

    // 公共方法
    utility::string_t send_message(std::string message);



private:
    // 私有构造函数
    GPTClient();
    // 私有析构函数
    ~GPTClient() = default;
    // 删除复制构造函数
    GPTClient(const GPTClient&) = delete;
    // 删除赋值运算符
    GPTClient& operator=(const GPTClient&) = delete;

    utility::string_t prompt;
    std::vector<Message> messages;
    utility::string_t model;
    utility::string_t apiKey;
    uri proxy;
    uri apiUrl;
};
class Message {
public:
    // 构造函数接受一个 json::value 对象
    Message(const json::value& jsonValue) {
        // 从 json::value 提取两个字符串
        if (jsonValue.has_field(U("role")) && jsonValue.has_field(U("content"))) {
            role = jsonValue.at(U("role")).as_string();
            content = jsonValue.at(U("content")).as_string();
        }
        else {
            throw std::invalid_argument("JSON does not have required fields");
        }
    }
    json::value toJson() {
        json::value jsonValue;
        jsonValue[U("role")] = json::value::string(role);
        jsonValue[U("content")] = json::value::string(content);
        return jsonValue;
    }

    utility::string_t role;
    utility::string_t content;
};