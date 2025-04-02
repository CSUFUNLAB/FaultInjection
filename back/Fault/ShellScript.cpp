#include "ShellScript.h"
#include "Log.h"
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
    // cmd递归删除文件夹太复杂了，还是手动删除吧
    string remote_path = string("/home/orangepi/data");
    string local_path = string("../../data/node_") + to_string(m_node_info->index);
    python_scp(remote_path, local_path, 0);
}

void ShellScript::copy_monitor_script(void)
{
    // cmd递归删除文件夹太复杂了，还是手动删除吧
    string remote_path = string("/home/orangepi/");
    string local_path = string("../../script/monitor_script");
    python_scp(remote_path, local_path, 1);
}

void ShellScript::chmod_monitor_script(void)
{
    m_is_root = true;
    string cmd = "chmod 777 /home/orangepi/monitor_script/*";
    python_ssh(cmd);
}

void ShellScript::update_monitor_script_thread(struct NodeManager::NodeInfo* node)
{
    LOG_INFO("copy_monitor_script");
    ShellScript* ssh = new ShellScript(node);
    ssh->copy_monitor_script();
    ssh->m_wait = 2; // 防止下面防止到之前ssh自动注销
    while (ssh->m_send_cmd) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LOG_INFO("chmod_monitor_script");
    ShellScript* ssh_1 = new ShellScript(node);
    ssh_1->chmod_monitor_script();
}

void ShellScript::update_monitor_script(struct NodeManager::NodeInfo* node)
{
    std::thread channel_chread(&ShellScript::update_monitor_script_thread, node);
    channel_chread.detach();
}

// 如果用了公钥，proxy jump 导致yes次数和密码次数不匹配，要手动连接这些板子
void ShellScript::first_connect(void)
{
    m_send_type = FIRST_CMD;
    python_ssh(string(""));
}


