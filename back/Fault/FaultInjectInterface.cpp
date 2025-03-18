#include "FaultInjection.h"
#include "FaultInjectInterface.h"
#include "Log.h"

#include <vector>

#include "RandomDataFlow.h"

using namespace std;
using namespace web;
using namespace web::json;
using namespace web::http;

void FaultInject::handlerData(http_request& message)
{
    json::value recvDatas = json::value::object();
    recvDatas = message.extract_json().get();
    // ��ȡ��message��web::json::value��ʽ
    ucout << "recvDatas:" << recvDatas << endl; // recvDatas:
    Json::Value recvJsonDatas = HandleJsonData(recvDatas);
    // recvDatas:{"faultParameter":"","faultType":"appdown","nodes":[{"key":"A","value":"1"}]}
    string fault_type = recvJsonDatas["faultType"].asString();
    LOG_INFO("{}", fault_type);
    string fault_para = recvJsonDatas["faultParameter"].asString();
    LOG_INFO("{}", fault_para);
    const Json::Value &nodeJsonDatas = recvJsonDatas["nodes"];
    FaultBase* fault = nullptr;
    int32_t ret = 0;
    int32_t node_num;
    for (const auto &node : nodeJsonDatas) {
        node_num = atoi(node["value"].asString().c_str());
        fault = FaultBase::create_fault(node_num, fault_type, fault_para);
        ret = fault->fault_injection();
        delete fault;
        if (ret != 0) {
            break;
        }
    }
    ret = ret == 0 ? 200 : 300 - ret;
    m_handler_info.code = ret;
}

void RandomFault::handlerData(http_request& message)
{
    random_fault();
    m_handler_info.code = 200;
}

#if 0
HandlerInfo FaultInjectInterface::handlerData(http_request message)
{
    json::value recvDatas = json::value::object();
    recvDatas = message.extract_json().get();
    // ��ȡ��message��web::json::value��ʽ
    ucout << "recvDatas:" << recvDatas  << endl; // recvDatas: {"faultParameter":"", "faultType" : "congest", "nodes" : [{"value":"1"}] }
    // ��ȡmessage��Json::Value��ʽ
    Json::Value recvJsonDatas = HandleJsonData(recvDatas);
    /* ����ʾ��
    recvJsonDatas: {
        "faultParameter" : "",// ���ϲ���
        "faultType" : "congest",// ��������
        "nodes" :// ���Ͻڵ�
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

    // �Ի�ȡ�Ĺ���ע�����ݽ��в���



    // ��ע��������ǰ��
    HandlerInfo handlerInfo;
    // ע��ɹ���code=200��ע��ʧ��codeΪ������msgΪ������Ϣ
    handlerInfo.code = 200;
    handlerInfo.msg = "ע��ɹ�";
    return handlerInfo;
}
#endif
