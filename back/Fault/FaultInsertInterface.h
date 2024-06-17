#pragma once
#include "FaultInterface.h"

class FaultInsertInterface:public FaultInterface
{
private:
	HandlerInfo handlerInfo;
	
public:
	HandlerInfo handlerData(http_request message);

};

