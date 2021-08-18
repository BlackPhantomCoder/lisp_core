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

CoreData::ObjPoll< DotPair> dp_pool = []() {
    CoreData::clear_pool_funcs().push_back([]() { 
        dp_pool.clear_free(); 
        });
    return CoreData::ObjPoll<DotPair>();
}();

CoreData::ObjPoll< Number> n_pool = []() {
    CoreData::clear_pool_funcs().push_back([]() { 
        n_pool.clear_free(); 
        });
    return CoreData::ObjPoll<Number>();
}();

CoreData::ObjPoll< Symbol> s_pool = []() {
    CoreData::clear_pool_funcs().push_back([]() { 
        s_pool.clear_free(); 
        });
    return CoreData::ObjPoll<Symbol>();
}();

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
