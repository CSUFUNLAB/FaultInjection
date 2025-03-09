#include "ShellScript.h"

ShellScriptSsh::ScriptCmdMap ShellScriptSsh::m_script_cmd_map = {
    {
        RECORD_MEM_STATUS,
        "./mem.sh"
    },
};

ShellScriptSsh::ShellScriptSsh(NodeManager::NodeInfo* node, ShellScriptType type) : SshSession(node)
{

}

