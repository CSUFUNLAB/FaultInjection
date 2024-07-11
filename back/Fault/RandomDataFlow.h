#pragma once

#include <stdint.h>
//#include "DataFlow.h"

class RandomDataFlow {
public:
    static RandomDataFlow *get_instance(void);
    bool m_generate_random_flow = true;
    void generate_pair_flow(void);

private:
    void generate_pair_flow_thread(void);
};

