#include "FuncHolder.h"
#include "Func.h"

#include "FuncID.h"
using namespace std;

FuncHolder::FuncHolder(Func* ptr):
    t_func(ptr)
{
}

FuncHolder::FuncHolder(FuncHolder&& rh)noexcept :
    t_func(rh.t_func)
{
    rh.t_func = nullptr;
}

FuncHolder::~FuncHolder()
{
    if(t_func)
        free_pool_by_funcid(t_func);
    t_func = nullptr;
}

FuncHolder& FuncHolder::operator=(FuncHolder&& rh)noexcept
{
    if (&rh != this) {
        t_func = rh.t_func;
        rh.t_func = nullptr;
    }
    return *this;
}

Func& FuncHolder::operator*()const
{
    return *t_func;
}

Func* FuncHolder::operator->() const
{
    return t_func;
}

Func* FuncHolder::get() const
{
    return t_func;
}
