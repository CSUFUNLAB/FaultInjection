#include "StringEscaping.h"

using namespace std;

StringEscaping &StringEscaping::get_instance(void)
{
    static StringEscaping p = StringEscaping();
    return p;
}

void StringEscaping::string_escaping(std::string &src, std::string &dst)
{
    for (char c : src) {
        auto ascii = m_string_escaping_map.find(c);
        if (ascii == m_string_escaping_map.end()) {
            dst += c;
        } else {
            dst += ascii->second;
        }
    }
}
