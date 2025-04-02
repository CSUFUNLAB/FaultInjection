#pragma once

#include <string>
#include <map>

class StringEscaping {
public:
    static StringEscaping &get_instance(void);
    void string_escaping(std::string &src, std::string &dst);

private:
    using StringEscapingMap = std::map<char, std::string>;
    StringEscapingMap m_string_escaping_map = {
        {'\x7b', "\\x7b"}, // {
        {'\x7d', "\\x7d"}, // }
        {'\x3a', "\\x3a"}, // :
        {'\x0a', "\\x0a"}, // \n
        {'\x22', "\\x22"}, // "
        /*
        如果要在shell使用具体功能，就不能转义
        {'\x24', "\\x24"}, // $
        {'\x28', "\\x28"}, // (
        {'\x29', "\\x29"}, // )
        {'\x25', "\\x25"}, // %
        {'\x2b', "\\x2b"}, // +
        */
    };
};
