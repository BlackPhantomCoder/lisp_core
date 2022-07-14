#include "SExpr.h"
#include "CoreData.h"
#include "DotPair.h"
#include "Number.h"
#include "Symbol.h"

using namespace std;


auto& dp_pool = CoreData::get_pool<DotPair>();
auto& n_pool = CoreData::get_pool<Number>();
auto& s_pool = CoreData::get_pool<Symbol>();


inline void pool_delete_dp(SExpr* ptr) {
    auto* p = (DotPair*)(ptr);
    p->clear();
    dp_pool.free(p);
}

inline void pool_delete_n(SExpr* ptr) {
    auto* p = (Number*)(ptr);
    p->clear();
    n_pool.free(p);
}

inline void pool_delete_s(SExpr* ptr) {
    auto* p = (Symbol*)(ptr);
    p->clear();
    s_pool.free(p);
}


void intrusive_ptr_add_ref(SExpr* p)
{
    ++p->t_count;
}

void intrusive_ptr_release(SExpr* p)
{
    --p->t_count;
    if (p->t_count == 0) {
        switch (p->t_type)
        {
            case SExpr::type::list:
                pool_delete_dp(p);
                break;
            case SExpr::type::symbol:
                pool_delete_s(p);
                break;
            case SExpr::type::number:
                pool_delete_n(p);
                break;
        default:
            throw logic_error("intrusive_ptr_release");
        }
    }
}

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
