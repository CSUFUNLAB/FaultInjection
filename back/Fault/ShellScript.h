#pragma once
#pragma once

#include "SshSession.h"

class ShellScript : public SshSession {
public:
    using SshSession::SshSession;
    void monitor_init(void);
    void kill_monitor(void);
    void collect_data(void);
};
