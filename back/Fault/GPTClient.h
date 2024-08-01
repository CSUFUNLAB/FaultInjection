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
    static GPTClient& getInstance(const std::string& apiKey = "");

    // ��������
    pplx::task<json::value> sendMessage(std::vector<json::value>& messages);

private:
    // ˽�й��캯��
    GPTClient(const std::string& apiKey);
    // ˽����������
    ~GPTClient() = default;
    // ɾ�����ƹ��캯��
    GPTClient(const GPTClient&) = delete;
    // ɾ����ֵ�����
    GPTClient& operator=(const GPTClient&) = delete;

    std::string apiKey;
    uri apiUrl;
};
