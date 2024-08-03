#include "GPTClient.h"
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include <iostream>
#include <vector>

using namespace utility;
using namespace std;
// ���췽��
GPTClient::GPTClient() {
    messages = {};
    load_config();
}

// ��ȡΨһʵ���ľ�̬����
GPTClient& GPTClient::get_instance() {
    static GPTClient instance;
    return instance;
}

// �������ò���
void GPTClient::load_config() {
    //�������ļ�
    std::ifstream file("./config.json");
    if (!file.is_open()) {
        throw "File Not Found: config.json not found.";
    }
    // ��ȡ�ļ����ַ�������
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    // ������ʼ��
    auto jsonValue = web::json::value::parse(buffer.str());
    prompt = jsonValue[U("prompt")].as_string();
    model = jsonValue[U("model")].as_string();
    apiKey = jsonValue[U("apiKey")].as_string();
    proxy = uri(jsonValue[U("proxy")].as_string());
    apiUrl = uri(jsonValue[U("apiUrl")].as_string());
    reset_messages();
}

// ��������¼
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

    //������Ϣ����Ի��б�
    messages.push_back(web::json::value::object({ { U("role"), web::json::value::string(U("user")) }, { U("content"),  web::json::value::string(conversions::to_string_t(message))} }));

    // ����HTTP�ͻ���
    web::http::client::http_client_config clientConfig;
    web::web_proxy proxyConfig(proxy);
    clientConfig.set_proxy(proxyConfig);
    web::http::client::http_client client(apiUrl, clientConfig);

    // ����HTTP����
    http_request request(methods::POST);
    request.headers().add(U("Authorization"), U("Bearer ") + conversions::to_string_t(apiKey));
    request.headers().set_content_type(U("application/json"));
    json::value payload;
    payload[U("model")] = json::value::string(model);
    payload[U("messages")] = json::value::array(messages);
    request.set_body(payload);

    // ������������
    client.request(request).then([this, &result](http_response response) -> pplx::task<json::value> {
        if (response.status_code() == status_codes::OK) {
            return response.extract_json().then([this, &result](json::value jsonResponse) -> json::value {
                //std::wcout << L"Response body: " << jsonResponse.serialize() << std::endl;
                // �����ص���Ϣ���������¼
                messages.push_back(web::json::value::object({ { U("role"), web::json::value::string(U("assistant")) }, { U("content"), jsonResponse[U("choices")][0][U("message")][U("content")] } }));
                result = jsonResponse[U("choices")][0][U("message")][U("content")].as_string();
                return jsonResponse;
                });
        }
        else {
            // ����δ���͵���Ϣ
            messages.pop_back();
            // ����һ���յ� JSON ������Ϊʧ�ܵ�Ĭ��ֵ
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