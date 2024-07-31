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

	virtual void handlerData(web::http::http_request &message); // 默认处理函数用于返回无操作的错误

	// // web::json:value 转换为 Json:Value （有些版本的cpprest好像不需要）
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

