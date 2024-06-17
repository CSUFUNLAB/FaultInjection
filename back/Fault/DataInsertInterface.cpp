#include "DataInsertInterface.h"
#include "ClientHandler.h"
#include <string>

#include "Log.h"

using namespace std;

HandlerInfo DataInsertInterface::handlerData(http_request message)
{
	json::value recvDatas = json::value::object();
	recvDatas = message.extract_json().get();
	// 获取了message的web::json::value格式
	ucout << "recvDatas:" << recvDatas << endl; // recvDatas:
	// 获取message的Json::Value格式
	Json::Value recvJsonDatas = HandleJsonData(recvDatas);

	/* 数据示例
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

	// 对获取的数据注入数据进行操作



	// 将注入结果返回前端
	HandlerInfo handlerInfo;
	// 注入成功：code=200；注入失败code为其他，msg为错误信息
	handlerInfo.code = 200;
	handlerInfo.msg = "注入成功";
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

	// 将注入结果返回前端
	HandlerInfo handlerInfo;
	// 注入成功：code=200；注入失败code为其他，msg为错误信息;code=601;结束传输
	handlerInfo.code = 200;
	handlerInfo.msg = jsonString;
	return handlerInfo;

	// 调用前端的服务端接口传递数据
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
