#pragma once
#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include <mutex>

#include "SshSession.h"

// 记录数据的脚步应该是一直运行的，所以应该由一个脚本触发所有脚本，而不是这边一个个触发

class ShellScriptSsh : public SshSession {
public:
    enum ShellScriptType {
        RECORD_MEM_STATUS,
    };

    ShellScriptSsh(NodeManager::NodeInfo* node, ShellScriptType type);

    using ScriptCmdMap = std::map<int32_t, std::string>;
    static ScriptCmdMap m_script_cmd_map;

};
