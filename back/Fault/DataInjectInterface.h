#pragma once
#include "FaultInterface.h"

class DataInjectInterface:public FaultInterface
{
private:
	HandlerInfo handlerInfo;

public:
	HandlerInfo handlerData(http_request message);

	HandlerInfo scan_node(http_request message);

	// ��ǰ�˴��ݽ��
	HandlerInfo faultResult(Json::Value recvJson);
};

