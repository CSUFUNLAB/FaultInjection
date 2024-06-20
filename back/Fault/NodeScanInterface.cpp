#include "NodeScanInterface.h"
#include "FaultInjection.h"
#include "Log.h"
#include "NodeManager.h"

HandlerInfo NodeScanInterface::handlerData(http_request message)
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

    // ��ע��������ǰ��
    HandlerInfo handlerInfo;
    // ע��ɹ���code=200��ע��ʧ��codeΪ������msgΪ������Ϣ
    handlerInfo.code = 200;
    handlerInfo.msg = nodes.toStyledString();
    LOG_INFO("{}", handlerInfo.msg);
    
    return handlerInfo;
}

