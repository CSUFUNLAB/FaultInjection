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
    // ��ȡΨһʵ���ľ�̬����
    static GPTClient& get_instance();

    // �������ò���
    void load_config();

    // ��������¼
    void reset_messages();

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
    std::vector<json::value> messages;
    utility::string_t model;
    utility::string_t apiKey;
    uri proxy;
    uri apiUrl;
};
