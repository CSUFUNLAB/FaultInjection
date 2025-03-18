#include "TestFunction.h"
#include "SshSession.h"
#include "ShellScript.h"
#include "Log.h"

using namespace std;
using namespace web;
using namespace web::json;
using namespace web::http;

void TestFunction::handlerData(http_request& message)
{
	ShellScript* ssh_1 = new ShellScript(&NodeManager::m_node_info_list[12]);
	ShellScript* ssh_2 = new ShellScript(&NodeManager::m_node_info_list[14]);
	ssh_1->kill_monitor();
	ssh_2->kill_monitor();
	ssh_1->collect_data();
	ssh_2->collect_data();
}