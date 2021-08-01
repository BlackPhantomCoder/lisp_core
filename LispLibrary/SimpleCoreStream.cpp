#include "SimpleCoreStream.h"
using namespace std;
using namespace CoreData;

SimpleInputCoreStream::SimpleInputCoreStream(std::istream& input, stream_read_mode mode):
    t_stream(input),
    t_mode(mode)
{
}

bool SimpleInputCoreStream::alive() const
{
    return bool(t_stream.get());
}

bool SimpleInputCoreStream::ready() const
{
    return bool(t_stream.get());
}

char SimpleInputCoreStream::get_char()
{
    return t_stream.get().get();
}

std::string SimpleInputCoreStream::get_line()
{
    string buf;
    getline(t_stream.get(), buf);
    return buf;
}

stream_read_mode SimpleInputCoreStream::get_mode() const
{
    return t_mode;
}

void SimpleInputCoreStream::unread_some(long val)
{
    t_stream.get().seekg(-val, ios::cur);
}

SimpleOutputCoreStream::SimpleOutputCoreStream(std::ostream& output):
    t_stream(output)
{
}

void SimpleOutputCoreStream::out_new_line(const std::string& val)
{
    t_stream.get() << val << endl;
}

void SimpleOutputCoreStream::out_without_new_line(const std::string& val)
{
    t_stream.get() << val;
}
