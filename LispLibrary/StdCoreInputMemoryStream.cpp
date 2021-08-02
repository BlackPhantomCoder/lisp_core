#include "StdCoreInputMemoryStream.h"
using namespace std;
#include <algorithm>
#include <iterator>

StdCoreInputMemoryStream::StdCoreInputMemoryStream(std::istream& input, CoreData::stream_read_mode mode) :
    t_mode(mode)
{
    std::string s;
    while (getline(input,s)) {
        t_lines.push_back(move(s));
    }
    t_pos = (begin(t_lines));
}

StdCoreInputMemoryStream::StdCoreInputMemoryStream(const StdCoreInputMemoryStream& rh):
    t_mode(rh.t_mode)
{
    t_lines = rh.t_lines;
    t_pos = begin(t_lines) + distance(begin(rh.t_lines), rh.t_pos);
}

bool StdCoreInputMemoryStream::alive() const
{
    return t_pos != end(t_lines);
}

bool StdCoreInputMemoryStream::ready() const
{
    return alive();
}

std::string StdCoreInputMemoryStream::t_read_line()
{
    if (t_pos == end(t_lines)) throw "StdCoreInputMemoryStream::t_read_line: out of range";
    std::string buf = *t_pos;
    ++t_pos;
    return buf;
}

CoreData::stream_read_mode StdCoreInputMemoryStream::get_mode() const
{
    return t_mode;
}

void StdCoreInputMemoryStream::rewind()
{
    t_clear_last();
    t_pos = begin(t_lines);
}