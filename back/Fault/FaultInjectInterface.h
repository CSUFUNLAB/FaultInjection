#pragma once
#include "FaultInterface.h"

class FaultInject : public FaultInterface {
public:
	void handlerData(web::http::http_request &message) override;
};

class RandomFault : public FaultInterface {
public:
	void handlerData(web::http::http_request &message) override;
};

