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

private:
    DataFile(NodeManager::NodeInfo *node);
    std::string serialie_data_info(DataInfo::IperfInfo& info);
    void daemon_threads(void);
    bool m_ssh_close = false;
};
