#pragma once
#include "FaultInterface.h"

class FaultInject : public FaultInterface {
public:
	void handlerData(http_request &message) override;
};

