#include "StdCoreInputMemoryStream.h"
using namespace std;
#include <algorithm>
#include <iterator>

vector<string> make_lines(std::istream& input) {
    vector<string> result;
    std::string s;
    while (getline(input, s)) {
        //s += '\n';
        result.push_back(move(s));
    }
    return result;
}

StdCoreInputMemoryStream::StdCoreInputMemoryStream(std::istream& input, CoreData::stream_read_mode mode):
    StdCoreInputValArrayInput<std::vector<std::string>>(make_lines(input), mode)
{
}
