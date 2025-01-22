#include "DataFile.h"
#include "Log.h"

using namespace std;

std::string DataFile::m_file_dir = "~/fault_data/";

string DataFile::serialie_int_data_info(uint32_t *num)
{
    return to_string(*num) + string(",");
}

DataFile::DataFile(NodeManager::NodeInfo *node) : SshSession(node)
{
    m_only_send = true;
    open();
    std::thread channel_chread(&DataFile::daemon_threads, this);
    channel_chread.detach();
}

void DataFile::cmd_end(void)
{
    close(); // 不detele this关了重新开
    m_ssh_close = true;
}

// 不知为何data file后面没有继续写入，那重新打开一下
void DataFile::daemon_threads(void)
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(600));
        LOG_INFO("close data file");
        m_broken_cmd = true;
        while (!m_ssh_close) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        m_ssh_close = false;
        m_broken_cmd = false;
        LOG_INFO("open data file");
        open();
    }
}

DataFile *DataFile::get_instance(void)
{
    static DataFile* data_file = new DataFile(&NodeManager::m_node_info_list[0]);
    return data_file;
}

string DataFile::serialie_data_info(DataInfo::IperfInfo &info)
{
    uint32_t transfer = static_cast<uint32_t>(info.transfer * info.transfer_unit);
    uint32_t band = static_cast<uint32_t>(info.band * info.band_unit);
    uint32_t lost = static_cast<uint32_t>(info.lost);
    uint32_t* int_point_arry[] = {
        &info.sec,
        &transfer,
        &band,
        &info.trans_type,
        &info.err,
        &info.rtry,
        &info.rtt,
        &lost,
    };
    string info_str = "\"";
    for (int i = 0; i < sizeof(int_point_arry) / sizeof(int_point_arry[0]); i++) {
        info_str += serialie_int_data_info(int_point_arry[i]);
    }
    info_str += string("\"");

    LOG_DEBUG("{}", info_str);
    return info_str;
}

void DataFile::send_data_info(DataInfo::IperfInfo &info)
{
    string file_name;
    if (info.is_client == 1) {
        file_name = string("node_") + to_string(info.self_node) + string("_") + to_string(info.pair_node) + string("_0.csv");
    } else {
        file_name = string("node_") + to_string(info.pair_node) + string("_") + to_string(info.self_node) + string("_1.csv");
    }
    string cmd_info = string("echo ") + serialie_data_info(info) + string(" >> ") + m_file_dir + file_name + string("\n");
    LOG_DEBUG("{}",cmd_info);
    only_send_cmd(cmd_info);
    // DataFileClient::get_instance()->send(info);
}

