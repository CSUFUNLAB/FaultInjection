#pragma once
#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include <mutex>

#include "SshSession.h"

// ��¼���ݵĽŲ�Ӧ����һֱ���еģ�����Ӧ����һ���ű��������нű������������һ��������

class ShellScriptSsh : public SshSession {
public:
    enum ShellScriptType {
        RECORD_MEM_STATUS,
    };

    ShellScriptSsh(NodeManager::NodeInfo* node, ShellScriptType type);

    using ScriptCmdMap = std::map<int32_t, std::string>;
    static ScriptCmdMap m_script_cmd_map;

};
