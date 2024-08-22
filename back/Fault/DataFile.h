#pragma once

#include "DataInfo.h"
#include "DataFlow.h"
#include "SshSession.h"
#include "NodeManager.h"

#include <string>

class DataFile : public SshSession {
public:
    static DataFile *get_instance(void); // 必须要使用这个来构造ssh，因为ssh目标节点是确定的
    void send_data_info(DataInfo::IperfInfo& info);
    static std::string m_file_dir;
    void cmd_end(void) override;
    static std::string serialie_int_data_info(uint32_t* num);

private:
    DataFile(NodeManager::NodeInfo *node);
    void daemon_threads(void);
    bool m_ssh_close = false;
    static std::string serialie_data_info(DataInfo::IperfInfo& info);
};

// http系列的头文件和一般头文件因为字符宽度问题不能放一起
#include "FaultClient.h"

class DataFileClient {
public:
    DataFileClient() { m_client = FaultClient::create_client(); };
    static DataFileClient* get_instance(void);
    void send(DataInfo::IperfInfo& info);
private:
    FaultClient* m_client;
};

