#include "GenerateData.h"
#include "NodeManager.h"
#include "ShellScript.h"
#include "RandomDataFlow.h"
#include "Log.h"

using namespace std;

GenerateData* GenerateData::get_instance(void)
{
    static GenerateData* p = new GenerateData();
    return p;
}

bool GenerateData::wait_all_cmd_success(void)
{
    int32_t wait = 90;
    while (SshSession::m_success_times < NodeManager::get_instance()->m_detected_num) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (wait-- < 0) {
            return false;
        }
    }
    return true;
}

bool GenerateData::start_collect(void)
{
    NodeManager::get_instance()->deal_detected_node([](struct NodeManager::NodeInfo* node){
        ShellScript *ssh = new ShellScript(node);
        ssh->monitor_init();
    });
    return wait_all_cmd_success();
}

bool GenerateData::stop_collect(void)
{
    NodeManager::get_instance()->deal_detected_node([](struct NodeManager::NodeInfo* node){
        ShellScript *ssh = new ShellScript(node);
        ssh->kill_monitor();
    });
    return wait_all_cmd_success();
}

bool GenerateData::downloading(void)
{
    DownLoadData::get_folder();
    NodeManager::get_instance()->deal_detected_node([](struct NodeManager::NodeInfo* node){
        DownLoadData *ssh = new DownLoadData(node);
        ssh->download_data();
    });
    return wait_all_cmd_success();
}

void GenerateData::generate_data_thread(void)
{

    for (int i = 0; i < 10; i++) {
        if (!start_collect()) {
            LOG_ERR("not all monitor init ");
            return;
        }

        random_flow();
        std::this_thread::sleep_for(std::chrono::seconds(60)); // 有的流结束的比较慢

        if (!stop_collect()) {
            LOG_ERR("not all monitor stop ");
            return;
        }
        if (!downloading()) {
            LOG_ERR("not all data download ");
            return;
        }
    }
    LOG_INFO("all data flow finish");
}

void GenerateData::generate_data(void)
{
    std::thread channel_chread(&GenerateData::generate_data_thread, GenerateData::get_instance());
    channel_chread.detach();
}

