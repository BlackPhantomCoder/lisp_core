#include "SymbolsFarm.h"
#include "Cell.h"
#include "Symbol.h"
#include "Number.h"
#include "DotPair.h"
#include "CoreData.h"
#include "SExprsFArm.h"
using namespace std;
using namespace CoreData;

SymbolsFarm::symbol_core SymbolsFarm::make_or_copy(std::string&& str)
{
    auto [it, reason] = t_symbol_to_it_and_count.insert({ move(str), { end(t_oblist), 0 } });
    if (reason) {
        t_oblist.push_front(it->first);
        it->second.first = begin(t_oblist);
        it->second.second = 0;
    }
    return symbol_core(it, *this);
}

SymbolsFarm::symbol_core SymbolsFarm::make_or_copy(const std::string& str)
{

    auto [it, reason] = t_symbol_to_it_and_count.insert({ str, { end(t_oblist), 0 } });
    if (reason) {
        t_oblist.push_front(it->first);
        it->second.first = begin(t_oblist);
        it->second.second = 0;
    }
    return symbol_core(it, *this);
}

void SymbolsFarm::del_symbol(symb_mp::iterator it)
{
    t_oblist.erase(it->second.first);
    t_symbol_to_it_and_count.erase(it);
}

const SymbolsFarm::oblist& SymbolsFarm::get_lst() const
{
    return t_oblist;
}

SymbolsFarm::symbol_core::symbol_core():
    t_owner(nullptr)
{
}

SymbolsFarm::symbol_core::symbol_core(symb_mp::iterator it, SymbolsFarm& owner)
    :
    t_base(it),
    t_owner(&owner)
{
    ++t_base->second.second;
}

SymbolsFarm::symbol_core::symbol_core(symbol_core&& rh) noexcept:
    t_base(rh.t_base),
    t_owner(rh.t_owner)
{
    rh.t_owner = nullptr;
}

SymbolsFarm::symbol_core::symbol_core(const symbol_core& rh)
    :
    t_base(rh.t_base),
    t_owner(rh.t_owner)
{
    ++t_base->second.second;
}

SymbolsFarm::symbol_core::~symbol_core()
{
    clear();
}

const char* SymbolsFarm::symbol_core::str() const
{
    return t_base->second.first->data();
}

size_t SymbolsFarm::symbol_core::size() const
{
    return t_base->second.first->size();
}

SymbolsFarm::symbol_core& SymbolsFarm::symbol_core::operator=(symbol_core&& rh) noexcept
{
    if (this != &rh) {
        t_base = rh.t_base;
        t_owner = rh.t_owner;
        rh.t_owner = nullptr;
    }
    return *this;
}

SymbolsFarm::symbol_core& SymbolsFarm::symbol_core::operator=(const symbol_core& rh)
{
    if (this != &rh) {
        t_base = rh.t_base;
        t_owner = rh.t_owner;
        ++t_base->second.second;
    }
    return *this;
}

bool SymbolsFarm::symbol_core::operator==(const symbol_core& rh) const
{
    return !this->empty() &&  t_owner== rh.t_owner && rh.t_base == t_base;
}

bool SymbolsFarm::symbol_core::empty() const
{
    return !t_owner;
}

void SymbolsFarm::symbol_core::clear()
{
    if (!this->empty()) {
        --t_base->second.second;
        if (t_base->second.second == 0) t_owner->del_symbol(t_base);
    }
    t_owner = nullptr;
}
