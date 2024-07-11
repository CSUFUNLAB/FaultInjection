#include "FaultInjectInterface.h"
#include "FaultInjection.h"
#include "Log.h"

#include <vector>

using namespace std;

void FaultInject::handlerData(http_request& message)
{
    json::value recvDatas = json::value::object();
    recvDatas = message.extract_json().get();
    // ��ȡ��message��web::json::value��ʽ
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
