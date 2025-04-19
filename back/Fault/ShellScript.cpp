#include "ShellScript.h"
#include "FaultInjection.h"
#include "Log.h"
#include <string>
// #include <filesystem>

namespace fs = std::filesystem;
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

void UpdateScript::copy_monitor_script(void)
{
    string remote_path = string("/home/orangepi/");
    string local_path = string("../../script/monitor_script");
    python_scp(remote_path, local_path, 1);
}

void UpdateScript::chmod_monitor_script(void)
{
    m_is_root = true;
    string cmd = "chmod 777 /home/orangepi/monitor_script/*";
    python_ssh(cmd);
}

void UpdateScript::update_monitor_script_thread(void)
{
    LOG_INFO("copy_monitor_script");
    copy_monitor_script();
    while (m_send_cmd) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LOG_INFO("chmod_monitor_script");
    chmod_monitor_script();
    while (m_send_cmd) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    delete this;
}

void UpdateScript::update_monitor_script(struct NodeManager::NodeInfo* node)
{
    UpdateScript* ssh = new UpdateScript(node);
    std::thread channel_chread(&UpdateScript::update_monitor_script_thread, ssh);
    channel_chread.detach();
}



// 如果用了公钥，proxy jump 导致yes次数和密码次数不匹配，要手动连接这些板子
void ShellScript::first_connect(void)
{
    m_send_type = FIRST_CMD;
    python_ssh(string(""));
}

float GetCpuLoad::get_cpu_load(void)
{
    m_send_type = SshSession::SHELL_CMD;
    python_ssh(". /home/orangepi/monitor_script/get_cpu_load.sh");
    while(m_send_cmd) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return m_cpu_loader;
}

void GetCpuLoad::read_echo(char* data)
{
    string result = data;
    size_t pos = result.find("cpu_load");
    if (pos == std::string::npos) {
        return;
    }
    size_t start = result.find('[');
    size_t end = result.find(']');

    if (start == std::string::npos || end == std::string::npos) {
        return;
    }
    std::string numStr = result.substr(start + 1, end - start - 1);
    m_cpu_loader = std::stof(numStr);
    LOG_INFO("cpu loader: {}", m_cpu_loader);
}

uint32_t DownLoadData::m_folder_index[20] = { 0 };
std::string DownLoadData::m_file_folder;

void DownLoadData::get_folder(void)
{
    string fault_type = FaultBase::m_fault_str[FaultBase::m_fault_type];
    string file_folder;
    do {
        uint32_t& index = m_folder_index[FaultBase::m_fault_node];
        file_folder = string("../../data/data_") + fault_type + string("_node_") + to_string(FaultBase::m_fault_node)
            + string("_") + to_string(index);
        index++;
    } while (fs::exists(file_folder));
    fs::create_directory(file_folder);
    LOG_INFO("create folder {}", file_folder);
    m_file_folder = file_folder;
}

void DownLoadData::download_data(void)
{
    // cmd递归删除文件夹太复杂了，还是手动删除吧，实际上由于做多次实验，只能手动删除
    string remote_path = string("/home/orangepi/data");
    string local_path = m_file_folder + string("/node_") + to_string(m_node_info->index);
    python_scp(remote_path, local_path, 0);
}

