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
    static std::string serialie_int_data_info(uint32_t* num);

private:
    DataFile(NodeManager::NodeInfo *node);
    void daemon_threads(void);
    bool m_ssh_close = false;
    static std::string serialie_data_info(DataInfo::IperfInfo& info);
};

// httpϵ�е�ͷ�ļ���һ��ͷ�ļ���Ϊ�ַ�������ⲻ�ܷ�һ��
#include "FaultClient.h"

class DataFileClient {
public:
    DataFileClient() { m_client = FaultClient::create_client(); };
    static DataFileClient* get_instance(void);
    void send(DataInfo::IperfInfo& info);
private:
    FaultClient* m_client;
};

