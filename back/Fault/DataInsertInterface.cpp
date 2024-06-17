#include "DataInsertInterface.h"
#include "ClientHandler.h"
#include <string>

#include "Log.h"

using namespace std;

HandlerInfo DataInsertInterface::handlerData(http_request message)
{
	json::value recvDatas = json::value::object();
	recvDatas = message.extract_json().get();
	// ��ȡ��message��web::json::value��ʽ
	ucout << "recvDatas:" << recvDatas << endl; // recvDatas:
	// ��ȡmessage��Json::Value��ʽ
	Json::Value recvJsonDatas = HandleJsonData(recvDatas);

	/* ����ʾ��
	recvJsonDatas: {
		"bandWidth" : 12,
		"nodeA" : "A",
		"nodeB" : "B",
		"sendTime" : 12
	}
	*/
	//int32_t node_a = recvJsonDatas["nodeA"].asInt();
	//int32_t node_b = recvJsonDatas["nodeB"].asInt();
	//string band_width = recvJsonDatas["bandWidth"].asString();
	//int32_t send_time = recvJsonDatas["sendTime"].asInt();

	string node_a = recvJsonDatas["nodeA"].asString();
	string node_b = recvJsonDatas["nodeB"].asString();
	string band_width = recvJsonDatas["bandWidth"].asString();
	string send_time = recvJsonDatas["sendTime"].asString();

	LOG_INFO("this is node: {} node: {} band width: {} send time:{}", node_a, node_b, band_width, send_time);

	//cout << "recvJsonDatas: " << recvJsonDatas << endl;

	// �Ի�ȡ������ע�����ݽ��в���



	// ��ע��������ǰ��
	HandlerInfo handlerInfo;
	// ע��ɹ���code=200��ע��ʧ��codeΪ������msgΪ������Ϣ
	handlerInfo.code = 200;
	handlerInfo.msg = "ע��ɹ�";
	return handlerInfo;
}



HandlerInfo DataInsertInterface::faultResult(Json::Value recvJson)
{
	/*
	* 
		recvJson = {
		"bandWidth": "8,3,5,12",
		"retry": "6,15,5,12"
		}
	*/
	string jsonString = recvJson.toStyledString();
	web::json::value webJson = web::json::value::parse(jsonString);

	// ��ע��������ǰ��
	HandlerInfo handlerInfo;
	// ע��ɹ���code=200��ע��ʧ��codeΪ������msgΪ������Ϣ;code=601;��������
	handlerInfo.code = 200;
	handlerInfo.msg = jsonString;
	return handlerInfo;

	// ����ǰ�˵ķ���˽ӿڴ�������
	/*
	std::wstring path = U("/api/fault_result");
	AlgoClient* mclient = new AlgoClient;
	http_response postResponse = mclient->MyClient(webJson, path);

	if (postResponse.status_code() == 200) {
		ucout << "Post response content: " << postResponse.extract_string().get() << std::endl;
	}
	else
	{
		ucout << "Post response content: " << postResponse.extract_string().get() << std::endl;
	}*/
}
