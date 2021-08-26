#include "CoreStreamInt.h"


bool CoreInputStreamInt::alive() const
{
    if (!t_alive()) return t_unreaded;
    return  true;
}

char CoreInputStreamInt::peek_char()
{
    if (t_unreaded) return t_last;
    return t_peek_char();
}

char CoreInputStreamInt::get_char()
{
    if (t_unreaded) {
        t_unreaded = false;
        return t_last;
    }
    t_last = t_read_char();
    return t_last;
}

void CoreInputStreamInt::unread_last()
{
    if (t_unreaded) throw "CoreInputStreamInt::unread_last_line: already unreaded";
    t_unreaded = true;
}


bool CoreInputStreamInt::is_unreaded() const
{
    return t_unreaded;
}
