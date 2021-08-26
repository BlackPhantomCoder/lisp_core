#include "StdCoreInputStream.h"
using namespace std;

StdCoreInputStream::StdCoreInputStream(std::istream& input, CoreData::stream_read_mode mode) :
    t_stream(input),
    t_mode(mode)
{
    //t_buf = input.get();
}

CoreData::stream_read_mode StdCoreInputStream::get_mode() const
{
    return t_mode;
}

char StdCoreInputStream::t_read_char()
{
    if (!alive()) throw "empty stream";
    /*auto buf = t_buf;
    t_buf = t_stream.get().get();
    return buf;*/
    auto buf = t_stream.get().get();
    if (!alive()) buf = ' ';
    return buf;
}

char StdCoreInputStream::t_peek_char()
{
    if (!alive()) throw "empty stream";
    auto buf = t_stream.get().get();
    if (!alive()) buf = ' ';
    t_stream.get().unget();
    return buf;
}

bool StdCoreInputStream::t_alive() const
{
    return bool(t_stream.get());
}
