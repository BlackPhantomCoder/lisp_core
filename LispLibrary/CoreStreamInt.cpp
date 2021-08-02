#include "CoreStreamInt.h"


std::string CoreInputStreamInt::get_line()
{
    if (t_unreaded) return t_last;
    t_last = t_read_line();
    return t_last;
}

void CoreInputStreamInt::unread_last_line()
{
    if (t_unreaded) throw "CoreInputStreamInt::unread_last_line: already unreaded";
}

void CoreInputStreamInt::erase_atlast(size_t val)
{
    if (t_unreaded) throw "CoreInputStreamInt::unread_atlast: already unreaded";
    if(val >= t_last.size()) throw "CoreInputStreamInt::unread_atlast: out of range";
    t_last.erase(0, val+1);
}

const std::string& CoreInputStreamInt::last_line()
{
    return t_last;
}

void CoreInputStreamInt::t_clear_last()
{
    t_last.clear();
}
