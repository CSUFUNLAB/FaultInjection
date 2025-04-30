#pragma once
#pragma once

#include "SshSession.h"
#include <string>

class ShellScript : public SshSession {
public:
    using SshSession::SshSession;
    void monitor_init(void);
    void apt_install(std::string cmd);
    void kill_monitor(void);
    void first_connect(void); // 电脑第一次连接某个板子的ssh的时候需要输入一个yes，手动连接可以看到
private:
};

class UpdateScript : public SshSession {
public:
    using SshSession::SshSession;
    void update_monitor_script_thread(void);
    void cmd_end(void) override {};

    static void update_monitor_script(struct NodeManager::NodeInfo* node);
private:
    void copy_monitor_script(void);
    void chmod_monitor_script(void);
};

class DownLoadData : public SshSession {
public:
    using SshSession::SshSession;
    void download_data(void);
    static void get_folder(void);
private:
    static std::string m_file_folder;
    static uint32_t m_folder_index[20]; // 目前没有超过20个节点，后面再自适应吧
};

class GetCpuLoad : public SshSession {
public:
    using SshSession::SshSession;
    float get_cpu_load(void);
    void read_echo(char *data) override;
    void cmd_end(void) override {};
private:
    float m_cpu_loader;
};
