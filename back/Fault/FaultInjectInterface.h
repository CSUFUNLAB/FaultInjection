#pragma once
#include "FaultInterface.h"

class FaultInjectInterface:public FaultInterface {
public:
	void handlerData(http_request message);
};

