#pragma once
#include <string>

#include "FrontInterface.h"

class InjectDataFlow : public FrontInterface {
public:
	void handlerData(web::http::http_request &message) override;
};

class ScanNode : public FrontInterface {
public:
	void handlerData(web::http::http_request &message) override;
};

class GenerateRandomFlow : public FrontInterface {
public:
	void handlerData(web::http::http_request &message) override;
};

