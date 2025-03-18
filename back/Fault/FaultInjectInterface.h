#pragma once
#include "FrontInterface.h"

class FaultInject : public FrontInterface {
public:
	void handlerData(web::http::http_request &message) override;
};

class RandomFault : public FrontInterface {
public:
	void handlerData(web::http::http_request &message) override;
};

