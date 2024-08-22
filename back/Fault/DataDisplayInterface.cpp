#include "FaultClient.h"
#include "DataFile.h"

#include "LOG.h"

using namespace std;

DataFileClient* DataFileClient::get_instance(void)
{
    static DataFileClient* p = new DataFileClient();
    return p;
}

void DataFileClient::send(DataInfo::IperfInfo &info)
{
    uint32_t transfer = static_cast<uint32_t>(info.transfer * info.transfer_unit);
    uint32_t band = static_cast<uint32_t>(info.band * info.band_unit);
    uint32_t lost = static_cast<uint32_t>(info.lost);
    uint32_t* int_point_arry[] = {
        &info.self_node,
        &info.pair_node,
        &info.is_client,
        &info.port,
        &info.trans_type,
        &info.sec,
        &info.err,
        &band,
        &transfer,
        &info.rtry,
        &info.rtt,
        &lost,
    };
    string info_str;
    for (int i = 0; i < sizeof(int_point_arry) / sizeof(int_point_arry[0]); i++) {
        info_str += DataFile::serialie_int_data_info(int_point_arry[i]);
    }

    LOG_DEBUG("{}", info_str);
    web::json::value webJson = web::json::value::parse(info_str);
    std::wstring path = U("/api/fault_result");
    m_client->rep_push(webJson, path);
}
