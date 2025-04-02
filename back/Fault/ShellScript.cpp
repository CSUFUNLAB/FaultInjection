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
    // cmd�ݹ�ɾ���ļ���̫�����ˣ������ֶ�ɾ����
    string remote_path = string("/home/orangepi/data");
    string local_path = string("../../data/node_") + to_string(m_node_info->index);
    python_scp(remote_path, local_path, 0);
}

void ShellScript::copy_monitor_script(void)
{
    // cmd�ݹ�ɾ���ļ���̫�����ˣ������ֶ�ɾ����
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
    ssh->m_wait = 2; // ��ֹ�����ֹ��֮ǰssh�Զ�ע��
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

// ������˹�Կ��proxy jump ����yes���������������ƥ�䣬Ҫ�ֶ�������Щ����
void ShellScript::first_connect(void)
{
    m_send_type = FIRST_CMD;
    python_ssh(string(""));
}


