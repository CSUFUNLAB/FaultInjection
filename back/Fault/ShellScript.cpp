#include "ShellScript.h"
#include <string>

using namespace std;

void ShellScript::monitor_init(void)
{
    m_is_root = true;
    string cmd = "sudo /home/orangepi/monitor_script/monitor_init.sh";
    python_ssh(cmd);
}

void ShellScript::kill_monitor(void)
{
    m_is_root = true;
    string cmd = "pkill -f monitor";
    python_ssh(cmd);
}

void ShellScript::collect_data(void)
{
    string remote_path = string("/home/orangepi/data");
    string local_path = string("../../data/node_") + to_string(m_node_info->index);
    python_scp(remote_path, local_path);
}

