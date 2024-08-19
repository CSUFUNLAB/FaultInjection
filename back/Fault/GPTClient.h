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
    // ��ȡΨһʵ���ľ�̬����
    static GPTClient& get_instance();

    // �������ò���
    void load_config();

    // ��������¼
    void reset_messages();

    // ������ӡmessages
    void print_messages() const;

    // ��������
    utility::string_t send_message(std::string message);



private:
    // ˽�й��캯��
    GPTClient();
    // ˽����������
    ~GPTClient() = default;
    // ɾ�����ƹ��캯��
    GPTClient(const GPTClient&) = delete;
    // ɾ����ֵ�����
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
    // ���캯������һ�� json::value ����
    Message(const json::value& jsonValue) {
        // �� json::value ��ȡ�����ַ���
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