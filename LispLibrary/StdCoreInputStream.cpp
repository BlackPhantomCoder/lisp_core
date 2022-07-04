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

int StdCoreInputStream::t_read_char()
{
    if (!t_stream.get()) {
        if (!t_eos) {
            t_eos = true;
            return -1;
        }
        throw "StdCoreInputStream: empty stream";
    }
    
    return t_stream.get().get();
}

int StdCoreInputStream::t_peek_char()
{
    if (!t_stream.get()) {
        if (!t_eos) {
            return -1;
        }
        throw "StdCoreInputStream: empty stream";
    }
    return t_stream.get().peek();
}

bool StdCoreInputStream::t_alive() const
{
    if (!t_stream.get() && t_eos) {
        return false;
    }
    return true;
}
