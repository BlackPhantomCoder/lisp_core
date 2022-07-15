#include "RangeBifunc.h"
#include "SupportFuncs.h"
#include "CoreData.h"
using namespace CoreData;
using namespace std;

RangeBiFunc::RangeBiFunc(CoreEnvironment& env, CarCdrIterator beg_it, CarCdrIterator end_it, bool forse_noeval) :
    Func(env)
{
    if (forse_noeval) {
        t_args = pair{ beg_it , end_it };
    }
    else {
        t_args = make_fnc<EvalQuoteRange>(env, beg_it, end_it);
    }
}


CarCdrIterator RangeBiFunc::t_args_beg()
{
    if (holds_alternative<Cell>(t_args)) return begin(get<Cell>(t_args));
    return get<pair<CarCdrIterator, CarCdrIterator>>(t_args).first;
}


CarCdrIterator RangeBiFunc::t_args_end()
{
    if (holds_alternative<Cell>(t_args)) return end(get<Cell>(t_args));
    return get<pair<CarCdrIterator, CarCdrIterator>>(t_args).second;
}


CarCdrConstIterator RangeBiFunc::t_args_beg() const
{
    if (holds_alternative<Cell>(t_args)) return begin(get<Cell>(t_args));
    return get<pair<CarCdrIterator, CarCdrIterator>>(t_args).first;
}


CarCdrConstIterator RangeBiFunc::t_args_end() const
{
    if (holds_alternative<Cell>(t_args)) return end(get<Cell>(t_args));
    return get<pair<CarCdrIterator, CarCdrIterator>>(t_args).second;
}


bool RangeBiFunc::t_eval_args()
{
    if (holds_alternative<monostate>(t_args)) {
        t_args = t_last_eval_val();
        return true;
    }
    if (holds_alternative<CoreData::HolderPtr>(t_args)) {
        t_eval_next(move(get<CoreData::HolderPtr>(t_args)));
        t_args = std::monostate{};
        return false;
    }
    return true;
}


void RangeBiFunc::t_internal_execute()
{
    return t_execute_func();
}