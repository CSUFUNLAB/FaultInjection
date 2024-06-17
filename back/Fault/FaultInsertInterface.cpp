#include "FaultInsertInterface.h"

HandlerInfo FaultInsertInterface::handlerData(http_request message)
{
	json::value recvDatas = json::value::object();
	recvDatas = message.extract_json().get();
	// 获取了message的web::json::value格式
	ucout << "recvDatas:" << recvDatas  << endl; // recvDatas: {"faultParameter":"", "faultType" : "congest", "nodes" : [{"value":"1"}] }
	// 获取message的Json::Value格式
	Json::Value recvJsonDatas = HandleJsonData(recvDatas);
	/*
	* 数据示例
	recvJsonDatas: {
		"faultParameter" : "",// 故障参数
		"faultType" : "congest",// 故障类型
		"nodes" :// 故障节点
			[
				{
					"value" : "1"
				}
			]
	}*/
	cout << "recvJsonDatas: " << recvJsonDatas << endl;

	// 对获取的故障注入数据进行操作



	// 将注入结果返回前端
	HandlerInfo handlerInfo;
	// 注入成功：code=200；注入失败code为其他，msg为错误信息
	handlerInfo.code = 200;
	handlerInfo.msg = "注入成功";
	return handlerInfo;
}
