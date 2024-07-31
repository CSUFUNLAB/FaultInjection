#pragma once
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace web::http::client;
using namespace web::http;

class FaultClient {
public:
	FaultClient(const std::wstring& url) : myClient(url) {};

	// 处理post请求，函数参数为：json数据，uri
	pplx::task<web::http::http_response> post(web::json::value& postData, const std::wstring& path);

	static void push(web::json::value &postData, const std::wstring &uri);

private:
	http_client myClient;
};

