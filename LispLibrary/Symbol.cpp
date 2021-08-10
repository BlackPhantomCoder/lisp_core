#include "Symbol.h"

using namespace std;

Symbol::~Symbol()
{
    clear();
}

bool Symbol::operator<(const Symbol& rh) const
{
    return t_data.str() < rh.t_data.str();
}

bool Symbol::operator==(const Symbol& rh) const
{
    return t_data == rh.t_data;
}

bool Symbol::operator!=(const Symbol& rh) const
{
    return !(t_data == rh.t_data);
}

size_t Symbol::size() const
{
    return t_data.size();
}

const char* Symbol::to_string() const
{
    return t_data.str();
}

bool Symbol::empty() const
{
    return t_data.empty();
}

void Symbol::clear()
{
    t_data.clear();
}

Symbol::Symbol(const SymbolsFarm::symbol_core& data) :
    t_data(data)
{
}

bool operator==(const char* lh, const Symbol& rh)
{
    return lh == rh.to_string();
}

bool operator==(const Symbol& lh, const char* rh)
{
    return lh.to_string() == rh;
}

bool operator==(const Symbol& lh, const std::string& rh)
{
    return lh.to_string() == rh;
}

bool operator==(const std::string& lh, const Symbol& rh)
{
    return lh == rh.to_string();
}

bool operator!=(const Symbol& lh, const char* rh)
{
    return lh.to_string() != rh;
}

bool operator!=(const char* lh, const Symbol& rh)
{
    return lh == rh.to_string();
}

bool operator!=(const Symbol& lh, const std::string& rh)
{
    return lh.to_string() != rh;
}

bool operator!=(const std::string& lh, const Symbol& rh)
{
    return lh != rh.to_string();
}