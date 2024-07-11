#include "FaultInjectInterface.h"
#include "FaultInjection.h"
#include "Log.h"

#include <vector>

using namespace std;

void FaultInject::handlerData(http_request& message)
{
    json::value recvDatas = json::value::object();
    recvDatas = message.extract_json().get();
    // 获取了message的web::json::value格式
    ucout << "recvDatas:" << recvDatas << endl; // recvDatas:
    Json::Value recvJsonDatas = HandleJsonData(recvDatas);
    // recvDatas:{"faultParameter":"","faultType":"appdown","nodes":[{"key":"A","value":"1"}]}
    string fault_type = recvJsonDatas["faultType"].asString();
    string fault_para = recvJsonDatas["faultParameter"].asString();
    Json::Value nodeJsonDatas = recvJsonDatas["node"];
    FaultBase* fault = nullptr;
    int32_t ret = 0;
    for (auto &node : nodeJsonDatas) {
        fault = FaultBase::create_fault(node["value"].asInt(), fault_type, fault_para);
        ret = fault->fault_injection();
        delete fault;
        if (ret != 0) {
            break;
        }
    }
    ret = ret == 0 ? 200 : 300 - ret;
    m_handler_info.code = ret;
}


#if 0
HandlerInfo FaultInjectInterface::handlerData(http_request message)
{
    json::value recvDatas = json::value::object();
    recvDatas = message.extract_json().get();
    // 获取了message的web::json::value格式
    ucout << "recvDatas:" << recvDatas  << endl; // recvDatas: {"faultParameter":"", "faultType" : "congest", "nodes" : [{"value":"1"}] }
    // 获取message的Json::Value格式
    Json::Value recvJsonDatas = HandleJsonData(recvDatas);
    /* 数据示例
    recvJsonDatas: {
        "faultParameter" : "",// 故障参数
        "faultType" : "congest",// 故障类型
        "nodes" :// 故障节点
            [
                {
                    "value" : "1"
                }
            ]
    }*/
    string fault_type = recvJsonDatas["faultType"].asString();
    string fault_para = recvJsonDatas["faultParameter"].asString();
    int32_t node_num = 0;
    FaultSsh* ssh = create_faultssh(node_num, fault_type, fault_para);

    cout << "recvJsonDatas: " << recvJsonDatas << endl;

    // 对获取的故障注入数据进行操作



    // 将注入结果返回前端
    HandlerInfo handlerInfo;
    // 注入成功：code=200；注入失败code为其他，msg为错误信息
    handlerInfo.code = 200;
    handlerInfo.msg = "注入成功";
    return handlerInfo;
}
#endif
