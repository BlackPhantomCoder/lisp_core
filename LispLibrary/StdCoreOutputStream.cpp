#include "StdCoreOutputStream.h"
using namespace std;

StdCoreOutputStream::StdCoreOutputStream(std::ostream& output) :
    t_stream(output)
{
}

void StdCoreOutputStream::out_new_line(const std::string& val)
{
    t_stream.get() << val << endl;
}

void StdCoreOutputStream::out_without_new_line(const std::string& val)
{
    t_stream.get() << val;
}
