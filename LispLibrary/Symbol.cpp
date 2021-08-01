#include "Symbol.h"
using namespace std;

Symbol::Symbol(const Symbol& rh) :
    t_data(rh.t_data)
{
}

Symbol& Symbol::operator=(const Symbol& rh)
{
    t_data = rh.t_data;
    return *this;
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

Symbol::Symbol(const Symbols::symbol_core& data) :
    t_data(data)
{
}

Symbol make_symbol(const std::string& data, Symbols& owner)
{
    return Symbol(owner.make_or_copy(data));
}

Symbol make_symbol(std::string&& data, Symbols& owner)
{
    return Symbol(owner.make_or_copy(move(data)));
}

bool operator==(const Symbol& lh, const std::string& rh)
{
    return lh.to_string() == rh;
}

bool operator==(const std::string& lh, const Symbol& rh)
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

std::ostream& operator<<(std::ostream& os, const Symbol& rh)
{
    os << rh.to_string();
    return os;
}

Symbols::symbol_core Symbols::make_or_copy(std::string&& str)
{
    auto [it, reason] = t_symbol_to_it_and_count.insert({ move(str), { end(t_oblist), 0 } });
    if (reason) {
        t_oblist.push_front(it->first);
        it->second.first = begin(t_oblist);
        it->second.second = 0;
    }
    return symbol_core(it, *this);
}

Symbols::symbol_core Symbols::make_or_copy(const std::string& str)
{
    auto [it, reason] = t_symbol_to_it_and_count.insert({ str, { end(t_oblist), 0 } });
    if (reason) {
        t_oblist.push_front(it->first);
        it->second.first = begin(t_oblist);
        it->second.second = 0;
    }
    return symbol_core(it, *this);
}

void Symbols::del_symbol(symb_mp::iterator it)
{
    t_oblist.erase(it->second.first);
    t_symbol_to_it_and_count.erase(it);
}

const Symbols::oblist& Symbols::get_lst() const
{
    return t_oblist;
}

Symbols::symbol_core::symbol_core(symb_mp::iterator it, Symbols& owner)
    :
    t_base(it),
    t_owner(owner)
{
    ++t_base->second.second;
}

Symbols::symbol_core::symbol_core(const symbol_core& rh)
    :
    t_base(rh.t_base),
    t_owner(rh.t_owner)
{
    ++t_base->second.second;
}

Symbols::symbol_core::~symbol_core()

{
    --t_base->second.second;
    if (t_base->second.second == 0) t_owner.get().del_symbol(t_base);
}

const char* Symbols::symbol_core::str() const
{
    return t_base->second.first->data();
}

size_t Symbols::symbol_core::size() const
{
    return t_base->second.first->size();
}

bool Symbols::symbol_core::operator==(const symbol_core& rh) const
{
    return &t_owner.get() == &rh.t_owner.get() && rh.t_base == t_base;
}