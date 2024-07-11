#pragma once
#include "FaultInterface.h"
#include <string>

class InjectDataFlow : public FaultInterface {
public:
	void handlerData(http_request &message) override;
};

class ScanNode : public FaultInterface {
public:
	void handlerData(http_request &message) override;
};

class GenerateRandomFlow : public FaultInterface {
public:
	void handlerData(http_request &message) override;
};
