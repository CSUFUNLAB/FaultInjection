#pragma once
#include "FaultInterface.h"

class FaultInjectInterface:public FaultInterface
{
private:
	HandlerInfo handlerInfo;
	
public:
	HandlerInfo handlerData(http_request message);

};

