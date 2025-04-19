#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/json.h>
#include <cpprest/details/basic_types.h>
#include <json/json.h>
#include <sys/stat.h>

using namespace web;
using namespace web::json;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class CommandHandle
{
public:
	// Ĭ�Ϲ��캯��
	CommandHandle() {}
	// ����һ��URL��Ϊ�����Ĺ��캯��
	CommandHandle(string_t url);

	// �򿪷�����
	pplx::task<void> open() { return myListener.open(); }
	// �رշ�����
	pplx::task<void> close() { return myListener.close(); }

private:
	// HTTP������
	http_listener myListener;

	// ����POST������
	void HandlePost(http_request message);
	void HandleGet(http_request message);
};