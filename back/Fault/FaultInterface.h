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
 * ����һ���ӿڴ������ĳ������нӿڴ�������Ӧ��������
 */


// ����ӿ����ݷ���ͨ����Ϣ
struct HandlerInfo
{
	int code;
	string msg;
};

class FaultInterface
{
public:
	// �ӿ����ݴ����麯��
	virtual HandlerInfo handlerData(http_request message) = 0;
	// �ӿڷ�����Ϣ�Ĵ��麯��
	virtual http_response HandleResponse(const HandlerInfo* dataStruct);
	// // web::json:value ת��Ϊ Json:Value ����Щ�汾��cpprest������Ҫ��
	Json::Value HandleJsonData(json::value requestJson);

private:

};

