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
    uint32_t* int_point_arry[7] = {
        &info.sec,
        &info.pair_node,
        &info.is_client,
        &info.trans_type,
        &info.err,
        &info.rtry,
        &info.rtt,
    };
    string info_str = "\"";
    for (int i = 0; i < 7; i++) {
        info_str += serialie_int_data_info(int_point_arry[i]);
    }
    info_str += to_string(info.transfer * info.transfer_unit) + string(",");
    info_str += to_string(info.band * info.band_unit) + string(",");
    info_str += to_string(info.lost) + string(",");
    info_str += string("\"");

    LOG_DEBUG("{}", info_str);
    return info_str;
}

void DataFile::send_data_info(DataInfo::IperfInfo &info)
{
    string file_name = m_file_name + to_string(info.self_node) + string(".csv");
    string cmd_info = string("echo ") + serialie_data_info(info) + string(" >> ") + file_name + string("\n");
    only_send_cmd(cmd_info);
}

