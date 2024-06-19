#pragma once
#include "FaultInterface.h"

class NodeScanInterface:public FaultInterface
{
private:
	HandlerInfo handlerInfo;

public:
	HandlerInfo handlerData(http_request message);
	// 往前端传递结果
	HandlerInfo faultResult(Json::Value recvJson);
};
