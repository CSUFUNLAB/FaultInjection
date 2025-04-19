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
	// 默认构造函数
	CommandHandle() {}
	// 接收一个URL作为参数的构造函数
	CommandHandle(string_t url);

	// 打开服务器
	pplx::task<void> open() { return myListener.open(); }
	// 关闭服务器
	pplx::task<void> close() { return myListener.close(); }

private:
	// HTTP监听器
	http_listener myListener;

	// 处理POST请求函数
	void HandlePost(http_request message);
	void HandleGet(http_request message);
};