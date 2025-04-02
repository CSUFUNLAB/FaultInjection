#pragma once
#pragma once

#include "SshSession.h"
#include <string>

class ShellScript : public SshSession {
public:
    using SshSession::SshSession;
    void monitor_init(void);
    void kill_monitor(void);
    void collect_data(void);
    void copy_monitor_script(void);
    void chmod_monitor_script(void);
    void first_connect(void); // ���Ե�һ������ĳ�����ӵ�ssh��ʱ����Ҫ����һ��yes���ֶ����ӿ��Կ���
    static void update_monitor_script(struct NodeManager::NodeInfo* node);
private:
    static void update_monitor_script_thread(struct NodeManager::NodeInfo* node);
};
