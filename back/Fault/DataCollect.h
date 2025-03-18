#pragma once

#include "FrontInterface.h"

class DataCollectCopy : public FrontInterface {
public:
	void handlerData(web::http::http_request &message) override;
};

class DataCollectBegin : public FrontInterface {
public:
	void handlerData(web::http::http_request &message) override;
};

class DataCollectEnd : public FrontInterface {
public:
	void handlerData(web::http::http_request &message) override;
};
