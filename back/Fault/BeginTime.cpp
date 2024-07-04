#include "BeginTime.h"

using namespace std;

BeginTime::BeginTime()
{
    start_time = chrono::system_clock::now();
}

uint32_t BeginTime::get_time()
{
    chrono::duration<long long> elapsed = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - start_time);
    return elapsed.count();

}

BeginTime* BeginTime::get_instance()
{
    static BeginTime* p = new BeginTime();
    return p;
}
