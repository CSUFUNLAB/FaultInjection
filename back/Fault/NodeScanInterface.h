#pragma once
#include "FaultInterface.h"

class NodeScanInterface:public FaultInterface
{
private:
	HandlerInfo handlerInfo;

public:
	HandlerInfo handlerData(http_request message);
	// ��ǰ�˴��ݽ��
	HandlerInfo faultResult(Json::Value recvJson);
};
