#include "SExpr.h"
#include "CoreData.h"
#include "DotPair.h"
#include "Number.h"
#include "Symbol.h"

void intrusive_ptr_add_ref(SExpr* p)
{
    ++p->t_count;
}

void intrusive_ptr_release(SExpr* p)
{
    --p->t_count;
    if (p->t_count == 0) {
        auto fnc = p->get_del_fnc();
        fnc(p);
    }
}

auto& dp_pool = CoreData::get_pool<DotPair>();
auto& n_pool = CoreData::get_pool<Number>();
auto& s_pool = CoreData::get_pool<Symbol>();

SExprShare make_SExprShare_list_noinit()
{
    return SExprShare(dp_pool.get_default());
}

SExprShare make_SExprShare_symb_noinit()
{
    return SExprShare(s_pool.get_default());
}

SExprShare make_SExprShare_numb_noinit()
{
    return SExprShare(n_pool.get_default());
}

void pool_delete_dp(SExpr* ptr) {
    auto* p = (DotPair*)(ptr);
    p->clear();
    dp_pool.free(p);
}

void pool_delete_n(SExpr* ptr) {
    auto* p = (Number*)(ptr);
    p->clear();
    n_pool.free(p);
}

void pool_delete_s(SExpr* ptr) {
    auto* p = (Symbol*)(ptr);
    p->clear();
    s_pool.free(p);
}

bool SExpr::is_real() const
{
    return false;
}

bool SExpr::is_integer() const
{
    return false;
}

bool SExpr::is_number() const
{
    return false;
}

bool SExpr::is_symbol() const
{
    return false;
}

bool SExpr::is_atom() const
{
    return false;
}

bool SExpr::is_list() const
{
    return false;
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

SExpr::SExpr(SExpr&& rh) noexcept:
    t_count(0)
{
}

SExpr::SExpr(const SExpr& rh) :
    t_count(0)
{
}

SExpr& SExpr::operator=(SExpr&& rh) noexcept
{
    if (this != &rh) {
        if (t_count != 0) {
            t_count = rh.t_count;
        }
    }
    return *this;
}

SExpr& SExpr::operator=(const SExpr& rh)
{
    if (this != &rh) {
        if (t_count == 0) {
            t_count = rh.t_count;
        }
    }
    return *this;
}
