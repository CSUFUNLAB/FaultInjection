#include "FrontInterface.h"
#include "Log.h"
#include <comutil.h>

#include "DataInjectInterface.h"
#include "FaultInjectInterface.h"

#include "TestFunction.h"
#include "DataCollect.h"

using namespace std;
using namespace web;
using namespace web::json;
using namespace web::http;


template<typename T>
FrontInterface* create_fault_interface(void)
{
    return new T();
}

FrontInterface::InterfaceFuncMap FrontInterface::m_interface_func_map = {
    {"/api/data_insert", create_fault_interface<InjectDataFlow>},
    {"/api/detect_nodes", create_fault_interface<ScanNode>},
    {"/api/random_insert", create_fault_interface<GenerateRandomFlow>},
    {"/api/fault_data_insert", create_fault_interface<FaultInject>},
    {"/api/scan_node", create_fault_interface<RandomFault>}, // 这个名字改一下
    {"/api/functional_test", create_fault_interface<TestFunction>},
    {"/api/start_collect", create_fault_interface<DataCollectBegin>},
    {"/api/end_collect", create_fault_interface<DataCollectEnd>},
    {"/api/load_collect", create_fault_interface<DataCollectCopy>},
};

std::map<int, std::string> FrontInterface::m_err_code_map {
    {300 + NORMAL_ERR, "error, please check log"},
    {300 + NO_NODE, "operate node not exist"},
    {300 + NO_EXIST_FLOW, "operate data flow not exist"},
    {300 + NO_OPERATE, "this operate not exist"},
    {300 + SSH_ERR, "ssh error, please check network"},
    {300 + ERR_CODE_BUTT, "not exist error code, please check code"},
};

FrontInterface* FrontInterface::fault_interface_factory(std::string& uri)
{
    auto it = m_interface_func_map.find(uri);
    if (it == m_interface_func_map.end()) {
        LOG_INFO("no this operate {}", uri);
        return new FrontInterface();
    }
    return (it->second)();
}

http_response FrontInterface::HandleResponse(void)
{
    int &code = m_handler_info.code;
    string &msg = m_handler_info.msg;
    if (code != 200 && code != 300 + DEFINE_ERR) {
        auto it = m_err_code_map.find(code);
        msg = m_err_code_map[300 + ERR_CODE_BUTT];
        if (it != m_err_code_map.end()) {
            msg = it->second;
        }
    }
    json::value responseJson;
    _bstr_t t = msg.c_str();
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

Json::Value FrontInterface::HandleJsonData(json::value requestJson)
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

void FrontInterface::handlerData(http_request& message)
{
    m_handler_info.code = 300 + NO_OPERATE;
}

http_response FrontInterface::handle(http_request &message)
{
    handlerData(message);
    return HandleResponse();
}

