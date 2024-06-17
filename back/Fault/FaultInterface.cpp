#include "FaultInterface.h"
#include <comutil.h>

http_response FaultInterface::HandleResponse(const HandlerInfo* dataStruct)
{
	string msg = dataStruct->msg;
	int code = dataStruct->code;

	json::value responseJson;

	const string temp = msg;
	_bstr_t t = temp.c_str();
	wchar_t* pwchar = (wchar_t*)t;
	wstring result = pwchar;

	responseJson[U("code")] = json::value::number(code);
	responseJson[U("message")] = json::value::string(result);
	http_response custom_response(status_codes::OK);

	custom_response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
	custom_response.headers().add(U("Access-Control-Allow-Methods"), U("*"));
	custom_response.headers().add(U("Access-Control-Allow-Headers"), U("*"));
	custom_response.headers().add(U("Content-Type"), U("application/json"));

	custom_response.set_body(responseJson);

	return custom_response;
}

Json::Value FaultInterface::HandleJsonData(json::value requestJson)
{
	// web::json:value 转换为 Json:Value 
	Json::Value changeJson;
	wstring jsonWstring = requestJson.serialize();

	// 将wstring  转换为 string
	_bstr_t temp = jsonWstring.c_str();
	char* pchar = (char*)temp;
	string jsonString = pchar;

	// 将jsonString解析为changeJson 
	Json::CharReaderBuilder builder;
	stringstream stream(jsonString);
	JSONCPP_STRING jsonParseErrors;
	Json::parseFromStream(builder, stream, &changeJson, &jsonParseErrors);

	return changeJson;
}
