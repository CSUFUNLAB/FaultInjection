#pragma once
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace web::http::client;
using namespace web::http;

class FaultClient {
public:
	FaultClient(const std::wstring& url) : myClient(url) {};

	// ����post���󣬺�������Ϊ��json���ݣ�uri
	pplx::task<web::http::http_response> post(web::json::value& postData, const std::wstring& path);

	static void push(web::json::value &postData, const std::wstring &uri);

	static FaultClient *create_client(void);
	void rep_push(web::json::value &postData, const std::wstring &uri); // Ƶ�����͵�ʱ��ʹ�ô˺���

private:
	http_client myClient;
};

