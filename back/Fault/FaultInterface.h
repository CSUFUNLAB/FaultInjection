#pragma once
#include <string>
#include <cpprest/json.h>
#include <json/json.h>
#include <cpprest/http_listener.h>
#include <map>
#include <functional>

class FaultInterface {
public:
	web::http::http_response handle(web::http::http_request &message);

	virtual void handlerData(web::http::http_request &message); // Ĭ�ϴ��������ڷ����޲����Ĵ���

	// // web::json:value ת��Ϊ Json:Value ����Щ�汾��cpprest������Ҫ��
	Json::Value HandleJsonData(web::json::value requestJson);
	static FaultInterface* fault_interface_factory(std::string &uri);

    struct HandlerInfo {
        int code;
        std::string msg;
    };
	struct HandlerInfo m_handler_info = {0};


private:
	using InterfaceFunc = std::function<FaultInterface *(void)>;
	using InterfaceFuncMap = std::map<std::string, InterfaceFunc>;
	static InterfaceFuncMap m_interface_func_map;
	static std::map<int, std::string> m_err_code_map;

	web::http::http_response HandleResponse(void);
};

