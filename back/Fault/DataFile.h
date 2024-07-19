#pragma once

#include "DataInfo.h"
#include "DataFlow.h"
#include "SshSession.h"
#include "NodeManager.h"

#include <string>

class DataFile : public SshSession {
public:
    static DataFile *get_instance(void); // ����Ҫʹ�����������ssh����ΪsshĿ��ڵ���ȷ����
    void send_data_info(DataInfo::IperfInfo& info);
    static std::string m_file_dir;
    void cmd_end(void) override;

private:
    DataFile(NodeManager::NodeInfo *node);
    std::string serialie_data_info(DataInfo::IperfInfo& info);
    void daemon_threads(void);
    bool m_ssh_close = false;
};
