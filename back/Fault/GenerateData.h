#pragma once

class GenerateData {
public:
    void generate_data(void);
    static GenerateData* get_instance(void);

private:
    bool start_collect(void);
    bool stop_collect(void);
    bool downloading(void);
    bool wait_all_cmd_success(void);
    void generate_data_thread(void);
};
