#include "TestFunction.h"
#include "SshSession.h"
#include "ShellScript.h"
#include "Log.h"

using namespace std;
using namespace web;
using namespace web::json;
using namespace web::http;

void test_func(void)
{
    string remote_path = string("/home/orangepi/monitor_script");
    string local_path = string("../../script/monitor_script");
    SshSession* ssh = new SshSession(&NodeManager::m_node_info_list[10]);
    ssh->python_scp(remote_path, local_path, 1);
    // popen test
    string cmd = "nohup timeout 10 \\\" iperf3 -s \\\"";
    // string cmd = "echo cmd_exe; iw dev wlan0";
    string ssh_cmd = string("ssh ") + string("orangepi") + string("@") + string("192.168.12.167") + string(" ")
        + string("\\\"") + cmd + string("\\\"");
    string all_cmd = string("python ../../script/ssh_connect.py ")
        + string("1 ")
        + string("1 ")
        + string("orangepi ")
        + string("\"") + ssh_cmd + string("\"");
    LOG_INFO("allcmd: {}", all_cmd);
    FILE* pipe = _popen(all_cmd.c_str(), "r");
    if (!pipe) {
        LOG_ERR("Failed to execute Python script!");
        return;
    }
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        LOG_INFO("{}", buffer);
    }
    int exit_code = _pclose(pipe);
    if (exit_code != 0) {
        LOG_ERR("Python script failed with exit code: {}", exit_code);
    }
}

void test_func_1(void)
{
    NodeManager::NodeInfo *node = &NodeManager::m_node_info_list[13];
    node->detected = true;
    node->ip = "192.168.12.186";
    // 还有ip
    ShellScript::update_monitor_script(node);
    // ShellScript* ssh = new ShellScript(node);
    //ssh->copy_monitor_script();
}

void TestFunction::handlerData(http_request& message)
{
    int32_t swich_cmd = 1;
    switch (swich_cmd) {
        case 0: // 第一次连接
            NodeManager::get_instance()->get_detected_node([](struct NodeManager::NodeInfo* node) {
                ShellScript* ssh = new ShellScript(node);
                ssh->first_connect();
            });break;
        case 1: // 更新脚本
            NodeManager::get_instance()->get_detected_node(ShellScript::update_monitor_script);break;
        case 2: // 测试命令
            NodeManager::get_instance()->get_detected_node([](struct NodeManager::NodeInfo* node) {
                ShellScript* ssh = new ShellScript(node);
                ssh->python_ssh("rm -rf /home/orangepi/monitor_script");
            });break;
        case 3:
            test_func_1(); break;
        default:
            test_func();
    }
}
