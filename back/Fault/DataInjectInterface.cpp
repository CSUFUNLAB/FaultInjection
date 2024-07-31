#include "DataInjectInterface.h"
#include "NodeManager.h"
#include "DataFlow.h"
#include "RandomDataFlow.h"
#include "FaultClient.h"
#include "DataFlow.h"

#include "Log.h"

using namespace std;
using namespace web;
using namespace web::json;
using namespace web::http;

void InjectDataFlow::handlerData(http_request &message)
{
    json::value recvDatas = json::value::object();
    recvDatas = message.extract_json().get();
    // 获取了message的web::json::value格式
    //ucout << "recvDatas:" << recvDatas << endl; // recvDatas:
    // 获取message的Json::Value格式
    Json::Value recvJsonDatas = HandleJsonData(recvDatas);

    int32_t node_src = recvJsonDatas["nodeSrc"].asInt();
    int32_t node_dst = recvJsonDatas["nodeDst"].asInt();
    string band_width = recvJsonDatas["bandWidth"].asString();
    string type = recvJsonDatas["type"].asString();
    int32_t send_time = recvJsonDatas["sendTime"].asInt();

    LOG_DEBUG("data flow: {} -> {} {} {} {}s", node_src, node_dst, type, band_width, send_time);
    DataFlow::FlowInfo flow_info = {
        NodeManager::get_node_info(node_src),
        NodeManager::get_node_info(node_dst),
        band_width,
        0,
        send_time,
        type,
        nullptr,
        nullptr,
    };
    int32_t ret = DataFlow::creat_data_flow(flow_info);
    ret = ret == 0 ? 200 : 300 - ret;
    m_handler_info.code = ret;
    //m_handler_info.msg = "success";
}

void ScanNode::handlerData(http_request &message)
{
    NodeManager::get_all_sta_ip();
    Json::Value nodes(Json::arrayValue);
    for (auto &node : NodeManager::m_node_info_list) {
        if (!node.detected) {
            LOG_INFO("node[{}] not detected", node.index);
            continue;
        }
        Json::Value node_json;
        node_json["node"] = node.index;
        node_json["deviceType"] = node.type;
        node_json["networkCard"] = node.dev;
        node_json["MAC"] = node.mac;
        node_json["IP"] = node.ip;
        if (node.up_linked != nullptr) {
            node_json["upstreamNode"] = node.up_linked->index;
        } else {
            node_json["upstreamNode"] = -1;
        }
        nodes.append(node_json);
    }

    m_handler_info.code = 200;
    m_handler_info.msg = nodes.toStyledString();
    LOG_DEBUG("{}", m_handler_info.msg);
}

void GenerateRandomFlow::handlerData(http_request& message)
{
    RandomDataFlow::get_instance()->generate_pair_flow();
    m_handler_info.code = 200;
    m_handler_info.msg = "success";
}

void DataFlowClient::send(DataFlow::FlowInfo& info)
{
    Json::Value flow_json;
    flow_json["nodeSrc"] = info.client->index;
    flow_json["nodeDst"] = info.server->index;
    flow_json["bandWidth"] = info.band;
    flow_json["type"] = info.type;
    flow_json["sendTime"] = info.time;
    web::json::value webJson = web::json::value::parse(flow_json.toStyledString());
    std::wstring path = U("/api/send_data_stream");
    FaultClient::push(webJson, path);
}

#if 0
HandlerInfo DataInjectInterface::faultResult(Json::Value recvJson)
{
    /*
    * 
        recvJson = {
        "bandWidth": "8,3,5,12",
        "retry": "6,15,5,12"
        }
    */
    string jsonString = recvJson.toStyledString();
    web::json::value webJson = web::json::value::parse(jsonString);

    // 将注入结果返回前端
    HandlerInfo m_handler_info;
    // 注入成功：code=200；注入失败code为其他，msg为错误信息;code=601;结束传输
    m_handler_info.code = 200;
    m_handler_info.msg = jsonString;
    return m_handler_info;

    // 调用前端的服务端接口传递数据
    /*
    std::wstring path = U("/api/fault_result");
    AlgoClient* mclient = new AlgoClient;
    http_response postResponse = mclient->MyClient(webJson, path);

    if (postResponse.status_code() == 200) {
        ucout << "Post response content: " << postResponse.extract_string().get() << std::endl;
    }
    else
    {
        ucout << "Post response content: " << postResponse.extract_string().get() << std::endl;
    }*/
}
#endif
