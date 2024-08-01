#include "GPTClient.h"
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include <iostream>
#include <vector>

using namespace utility;

GPTClient::GPTClient(const std::string& apiKey)
    : apiKey(apiKey), apiUrl(U("https://api.openai.com/v1/chat/completions")) {}

// ��ȡΨһʵ���ľ�̬����
GPTClient& GPTClient::getInstance(const std::string& apiKey) {
    static GPTClient instance(apiKey);
    return instance;
}

pplx::task<json::value> GPTClient::sendMessage(std::vector<json::value>& messages) {
    json::value payload;
    payload[U("model")] = json::value::string(U("gpt-4"));
    payload[U("messages")] = json::value::array(messages);

    http_client client(apiUrl);
    http_request request(methods::POST);
    request.headers().add(U("Authorization"), U("Bearer ") + conversions::to_string_t(apiKey));
    request.headers().set_content_type(U("application/json"));
    request.set_body(payload);
    return client.request(request).then([&messages](http_response response) -> pplx::task<json::value> {
        if (response.status_code() == status_codes::OK) {
            return response.extract_json().then([&messages](json::value jsonResponse) -> json::value {
                // ����ȡ�� JSON ��ӵ� messages ��
                messages.push_back(jsonResponse);
                // ������ȡ�� JSON ����
                return jsonResponse;
             });
        }
        else {
            // ����һ���յ� JSON ������Ϊʧ�ܵ�Ĭ��ֵ
            return pplx::task_from_result(json::value());
        }
    }).then([](pplx::task<json::value> task) -> json::value {
            try {
                return task.get();
            }
            catch (const std::exception& e) {
                // �����쳣���
                return json::value();
            }
        });
}