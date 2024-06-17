#pragma once
#include <string>
#include <cpprest/json.h>
#include <json/json.h>
#include <cpprest/http_listener.h>

using namespace std;
using namespace web;
using namespace web::json;
using namespace http;


/**
 * 代表一个接口处理函数的抽象，所有接口处理函数都应是其子类
 */


// 处理接口数据返回通用信息
struct HandlerInfo
{
	int code;
	string msg;
};

class FaultInterface
{
public:
	// 接口数据处理纯虚函数
	virtual HandlerInfo handlerData(http_request message) = 0;
	// 接口返回信息的纯虚函数
	virtual http_response HandleResponse(const HandlerInfo* dataStruct);
	// // web::json:value 转换为 Json:Value （有些版本的cpprest好像不需要）
	Json::Value HandleJsonData(json::value requestJson);

private:

};

