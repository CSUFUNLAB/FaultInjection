#include "GPTClient.h"
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include <iostream>
#include <vector>

using namespace utility;
using namespace std;
// 构造方法
GPTClient::GPTClient() {
    messages = {};
    load_config();
}

// 获取唯一实例的静态方法
GPTClient& GPTClient::get_instance() {
    static GPTClient instance;
    return instance;
}

// 加载配置参数
void GPTClient::load_config() {
    //打开配置文件
    std::ifstream file("./config.json");
    if (!file.is_open()) {
        throw "File Not Found: config.json not found.";
    }
    // 读取文件到字符串流中
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    // 变量初始化
    auto jsonValue = web::json::value::parse(buffer.str());
    prompt = jsonValue[U("prompt")].as_string();
    model = jsonValue[U("model")].as_string();
    apiKey = jsonValue[U("apiKey")].as_string();
    proxy = uri(jsonValue[U("proxy")].as_string());
    apiUrl = uri(jsonValue[U("apiUrl")].as_string());
    reset_messages();
}

// 清空聊天记录
void GPTClient::reset_messages() {
    if (prompt.empty()) {
        messages.clear();
    }
    else {
        messages.clear();
        messages.push_back(web::json::value::object({ { U("role"), web::json::value::string(U("system")) }, { U("content"), web::json::value::string(prompt) } }));
    }
}

utility::string_t GPTClient::send_message(std::string message) {
    utility::string_t result;

    //将新消息存入对话列表
    messages.push_back(web::json::value::object({ { U("role"), web::json::value::string(U("user")) }, { U("content"),  web::json::value::string(conversions::to_string_t(message))} }));

    // 创建HTTP客户端
    web::http::client::http_client_config clientConfig;
    web::web_proxy proxyConfig(proxy);
    clientConfig.set_proxy(proxyConfig);
    web::http::client::http_client client(apiUrl, clientConfig);

    // 创建HTTP请求
    http_request request(methods::POST);
    request.headers().add(U("Authorization"), U("Bearer ") + conversions::to_string_t(apiKey));
    request.headers().set_content_type(U("application/json"));
    json::value payload;
    payload[U("model")] = json::value::string(model);
    payload[U("messages")] = json::value::array(messages);
    request.set_body(payload);

    // 发起网络请求
    client.request(request).then([this, &result](http_response response) -> pplx::task<json::value> {
        if (response.status_code() == status_codes::OK) {
            return response.extract_json().then([this, &result](json::value jsonResponse) -> json::value {
                //std::wcout << L"Response body: " << jsonResponse.serialize() << std::endl;
                // 将返回的消息存入聊天记录
                messages.push_back(web::json::value::object({ { U("role"), web::json::value::string(U("assistant")) }, { U("content"), jsonResponse[U("choices")][0][U("message")][U("content")] } }));
                result = jsonResponse[U("choices")][0][U("message")][U("content")].as_string();
                return jsonResponse;
                });
        }
        else {
            // 撤回未发送的消息
            messages.pop_back();
            // 返回一个空的 JSON 对象作为失败的默认值
            return pplx::task_from_result(json::value());
        }
        }).then([](pplx::task<json::value> task) {
            try {
                return task.get();
            }
            catch (const std::exception& e) {
                cout << e.what() << endl;
            }
            }).wait();
        return result;
}