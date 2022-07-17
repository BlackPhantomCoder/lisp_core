#include "RangeBifunc.h"
#include "SupportFuncs.h"
#include "CoreData.h"
using namespace CoreData;
using namespace std;

RangeBiFunc::RangeBiFunc(func_id id, CarCdrIterator beg_it, CarCdrIterator end_it, bool forse_noeval):
    Func(id)
{
    if (forse_noeval) {
        t_args = pair{ beg_it , end_it };
    }
    else {
        t_args = make_fnc<EvalQuoteRange>(beg_it, end_it);
    }
}

CarCdrIterator RangeBiFunc::args_beg()
{
    if (holds_alternative<Cell>(t_args)) return begin(get<Cell>(t_args));
    return get<pair<CarCdrIterator, CarCdrIterator>>(t_args).first;
}

CarCdrIterator RangeBiFunc::args_end()
{
    if (holds_alternative<Cell>(t_args)) return end(get<Cell>(t_args));
    return get<pair<CarCdrIterator, CarCdrIterator>>(t_args).second;
}

CarCdrConstIterator RangeBiFunc::args_beg() const
{
    if (holds_alternative<Cell>(t_args)) return begin(get<Cell>(t_args));
    return get<pair<CarCdrIterator, CarCdrIterator>>(t_args).first;
}

CarCdrConstIterator RangeBiFunc::args_end() const
{
    if (holds_alternative<Cell>(t_args)) return end(get<Cell>(t_args));
    return get<pair<CarCdrIterator, CarCdrIterator>>(t_args).second;
}

void RangeBiFunc::t_eval_args()
{
    if (holds_alternative<monostate>(t_args)) {
        t_args = s_last_eval_val();
    }
    else if (holds_alternative<CoreData::HolderPtr>(t_args)) {
        f_eval_next(move(get<CoreData::HolderPtr>(t_args)));
        t_args = std::monostate{};
    }
}