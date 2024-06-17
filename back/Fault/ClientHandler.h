#pragma once

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace std;
using namespace web::http::client;
using namespace web::http;

class ClientHandler
{
public:
	ClientHandler(const wstring& url);
	// 处理post请求，函数参数为：json数据，uri
	pplx::task<http_response> post(web::json::value postData, const std::wstring path);

private:
	http_client myClient;

};

class AlgoClient
{
public:
	http_response MyClient(web::json::value postData, wstring uri);
};