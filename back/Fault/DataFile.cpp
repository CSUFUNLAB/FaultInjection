#include "DataFile.h"
#include "Log.h"

using namespace std;

//string DataFile::m_file_path = "~/fault_data/";

static string serialie_int_data_info(uint32_t *num)
{
    return to_string(*num) + string(",");
}

DataFile::DataFile(NodeManager::NodeInfo *node) : SshSession(node)
{
    m_only_send = true;
    open();
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
        &info.pair_node,
        &info.is_client,
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
    string file_name = m_file_name + to_string(info.self_node) + string(".csv");
    string cmd_info = string("echo ") + serialie_data_info(info) + string(" >> ") + file_name + string("\n");
    LOG_DEBUG("{} {}", file_name, cmd_info);
    only_send_cmd(cmd_info);
}

