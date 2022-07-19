#include "SExpr.h"
#include "CoreData.h"
#include "DotPair.h"
#include "Number.h"
#include "Symbol.h"

using namespace std;
using namespace CoreData;

void pn_pool_free_dpair(SExpr* obj)
{
    static auto& pool = get_pn_pool<DotPair>();
    auto* dptr = (DotPair*)obj;
    dptr->clear();
    pool.free(dptr);
}

constexpr void (*intrusive_ptr_release_pool_free_mapper[]) (SExpr*) =
{ pn_pool_free_dpair, pn_pool_free_derived<Number, SExpr>, pn_pool_free_derived<Symbol, SExpr> };


void intrusive_ptr_add_ref(SExpr* p)
{
    ++p->t_count;
}

void intrusive_ptr_release(SExpr* p)
{
    --p->t_count;
    if (p->t_count == 0) {
        intrusive_ptr_release_pool_free_mapper[(unsigned)p->t_type](p);
    }
}

bool SExpr::is_real() const
{
    return is_number() && to_number().t_is_real();
}

bool SExpr::is_integer() const
{
    return is_number() && to_number().t_is_integer();
}

bool SExpr::is_number() const
{
    return t_type == type::number;
}

bool SExpr::is_symbol() const
{
    return t_type == type::symbol;
}

bool SExpr::is_atom() const
{
    return is_symbol() || is_number();
}

bool SExpr::is_list() const
{
    return t_type == type::list;
}

const Number& SExpr::to_number() const
{
    return *((Number*)this);
}

const Symbol& SExpr::to_symbol() const
{
    return *((Symbol*)this);
}

const DotPair& SExpr::to_list() const
{
    return *((DotPair*)this);
}

Number& SExpr::to_number()
{
    return *((Number*)this);
}

Symbol& SExpr::to_symbol()
{
    return *((Symbol*)this);
}

DotPair& SExpr::to_list()
{
    return *((DotPair*)this);
}

SExpr::SExpr(type type):
    t_type(type)
{
    if (t_type == type::none) {
        auto i = 0;
    }
}

SExpr::SExpr(SExpr&& rh) noexcept:
    t_count(0),
    t_type(rh.t_type)
{
}

SExpr::SExpr(const SExpr& rh) :
    t_count(0),
    t_type(rh.t_type)
{
}

SExpr& SExpr::operator=(SExpr&& rh) noexcept
{
    if (this != &rh) {
        if (t_count != 0) {
            t_count = rh.t_count;
            t_type = rh.t_type;
        }
    }
    return *this;
}

SExpr& SExpr::operator=(const SExpr& rh)
{
    if (this != &rh) {
        if (t_count == 0) {
            t_count = rh.t_count;
            t_type = rh.t_type;
        }
    }
    return *this;
}
