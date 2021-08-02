#include "StdCoreInputStream.h"
using namespace std;

StdCoreInputStream::StdCoreInputStream(std::istream& input, CoreData::stream_read_mode mode) :
    t_stream(input),
    t_mode(mode)
{
}

bool StdCoreInputStream::alive() const
{
    return bool(t_stream.get());
}

bool StdCoreInputStream::ready() const
{
    return bool(t_stream.get());
}

std::string StdCoreInputStream::t_read_line()
{
    std::string buf;
    getline(t_stream.get(), buf);
    return buf;
}

CoreData::stream_read_mode StdCoreInputStream::get_mode() const
{
    return t_mode;
}