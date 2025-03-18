#pragma once

#include "FrontInterface.h"

class TestFunction : public FrontInterface {
public:
	void handlerData(web::http::http_request &message) override;
};
