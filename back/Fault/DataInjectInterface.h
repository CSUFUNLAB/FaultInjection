#pragma once
#include <string>

#include "FaultInterface.h"

class InjectDataFlow : public FaultInterface {
public:
	void handlerData(web::http::http_request &message) override;
};

class ScanNode : public FaultInterface {
public:
	void handlerData(web::http::http_request &message) override;
};

class GenerateRandomFlow : public FaultInterface {
public:
	void handlerData(web::http::http_request &message) override;
};

