#include "Symbol.h"
#include "CoreData.h"

using namespace std;

Symbol::Symbol(const SymbolsFarm::symbol_core& data) :
    SExpr(SExpr::type::symbol),
    t_data(data)
{
}

Symbol::Symbol(SymbolsFarm::symbol_core&& data) :
    SExpr(SExpr::type::symbol),
    t_data(move(data))
{
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


bool operator==(const char* lh, const Symbol& rh)
{
    return strcmp(lh, rh.to_string()) == 0;
}

bool operator==(const Symbol& lh, const char* rh)
{
    return strcmp(lh.to_string(), rh) == 0;
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
    return strcmp(lh.to_string(), rh) != 0;
}

bool operator!=(const char* lh, const Symbol& rh)
{
    return strcmp(lh, rh.to_string()) != 0;
}

bool operator!=(const Symbol& lh, const std::string& rh)
{
    return lh.to_string() != rh;
}

bool operator!=(const std::string& lh, const Symbol& rh)
{
    return lh != rh.to_string();
}