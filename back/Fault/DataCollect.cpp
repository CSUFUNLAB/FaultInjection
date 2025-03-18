#include "DataCollect.h"
#include "ShellScript.h"
#include "NodeManager.h"

using namespace std;
using namespace web;
using namespace web::json;
using namespace web::http;

void DataCollectCopy::handlerData(http_request& message)
{
    NodeManager::get_instance()->get_detected_node([](struct NodeManager::NodeInfo* node){
        ShellScript *ssh = new ShellScript(node);
        ssh->collect_data();
    });
}

void DataCollectBegin::handlerData(http_request& message)
{
    NodeManager::get_instance()->get_detected_node([](struct NodeManager::NodeInfo* node){
        ShellScript *ssh = new ShellScript(node);
        ssh->monitor_init();
    });
}

void DataCollectEnd::handlerData(http_request& message)
{
    NodeManager::get_instance()->get_detected_node([](struct NodeManager::NodeInfo* node){
        ShellScript *ssh = new ShellScript(node);
        ssh->kill_monitor();
    });
}
