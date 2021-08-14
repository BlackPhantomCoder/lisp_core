#include "BiFunc.h"
#include "CoreEnv.h"
#include "Bifuncs.h"

using namespace std;

Func::Func(CoreEnvironment& env):
    t_env_data(env),
    t_stage(stages::not_started),
    t_next_func_buf(nullptr, nullptr)
{
}

stages Func::execute()
{
    if (t_stage == stages::executed) throw "";
    if (t_stage == stages::not_started) {
        t_stage = stages::intermediate;
        auto buf = t_init_before_args();
        if (buf != stages::intermediate) return t_cycle();
    }
    if (t_stage == stages::intermediate) {
        if (!t_eval_args_flag) {
            while(!t_eval_args_flag && t_stage == stages::intermediate)
                t_eval_args_flag = t_eval_args();
            return t_cycle();
        }
        if (!t_inited) {
            t_inited = true;
            auto buf = t_init_after_args();
            if (buf != stages::intermediate) return t_cycle();
        }
        return t_internal_execute();
    }
    if (t_stage == stages::need_external) {
            if (t_eval_next_flag) {
                t_eval_next_flag = false;
                t_stage = stages::intermediate;

                return t_stage;
            }
            else {
                //return t_return(*t_next_result_buf);
                t_stage = stages::executed;

                return t_stage;
            }
    }

    throw "";
}

stages Func::stage() const
{
    return t_stage;
}

void Func::push_next(const Cell& result)
{
    if (t_stage != stages::need_external) throw "";
    t_buf = result;
    t_next_func_buf.reset();
}

FuncInt& Func::get_next()
{
    if (t_stage != stages::need_external || !t_next_func_buf) throw "";
    return (*t_next_func_buf)();
}

Cell Func::result() const
{
    if (!t_buf) throw "";
    return *t_buf;
}

CoreEnvironment& Func::t_env()
{
    return t_env_data;
}

const CoreEnvironment& Func::t_env() const
{
    return t_env_data;
}

stages Func::t_return(const Cell& result)
{
    t_buf = result;
    t_stage = stages::executed;
    return t_stage;
}

stages Func::t_return_next(CoreData::HolderPtr&& next)
{

    t_next_func_buf = move(next);
    t_stage = stages::need_external;
    return t_stage;
}

stages Func::t_eval_next(CoreData::HolderPtr&& next)
{
    t_eval_next_flag = true;
    t_next_func_buf = move(next);
    t_stage = stages::need_external;
    return t_stage;
}

stages Func::t_cycle()
{
    return t_stage;
}

Cell Func::t_last_eval_val()
{
    if (!t_buf) throw "";
    return *t_buf;
}

RangeBiFunc::RangeBiFunc(CoreEnvironment& env, CarCdrIterator beg_it, CarCdrIterator end_it, bool forse_noeval):
    Func(env)
{
    if (forse_noeval) {
        t_args = pair{ beg_it , end_it };
    }
    else {
        t_args_eval_func = make_fnc<EvalQuoteRange>(env, beg_it, end_it);
    }
}

CarCdrIterator RangeBiFunc::t_args_beg()
{
    if (holds_alternative<monostate>(t_args)) throw "";
    if (holds_alternative<Cell>(t_args)) return begin(get<Cell>(t_args));
    return get<pair<CarCdrIterator, CarCdrIterator>>(t_args).first;
}

CarCdrIterator RangeBiFunc::t_args_end()
{
    if (holds_alternative<monostate>(t_args)) throw "";
    if (holds_alternative<Cell>(t_args)) return end(get<Cell>(t_args));
    return get<pair<CarCdrIterator, CarCdrIterator>>(t_args).second;
}

CarCdrConstIterator RangeBiFunc::t_args_beg() const
{
    if (holds_alternative<monostate>(t_args)) throw "";
    if (holds_alternative<Cell>(t_args)) return begin(get<Cell>(t_args));
    return get<pair<CarCdrIterator, CarCdrIterator>>(t_args).first;
}

CarCdrConstIterator RangeBiFunc::t_args_end() const
{
    if (holds_alternative<monostate>(t_args)) throw "";
    if (holds_alternative<Cell>(t_args)) return end(get<Cell>(t_args));
    return get<pair<CarCdrIterator, CarCdrIterator>>(t_args).second;
}

bool RangeBiFunc::t_eval_args()
{
    if (!holds_alternative<monostate>(t_args)) return true;
    if (!t_args_eval_func) {
        t_args = t_last_eval_val();
        return true;
    }
    t_eval_next(move(*t_args_eval_func));
    t_args_eval_func.reset();
    return false;
}

stages RangeBiFunc::t_internal_execute()
{
    if (holds_alternative<monostate>(t_args)) throw "";
    return t_execute_func();
}

RangeNBiFunc::RangeNBiFunc(CoreEnvironment& env, CarCdrIterator beg_it, CarCdrIterator end_it):
    Func(env),
    t_args({ beg_it , end_it })
{
}

CarCdrIterator RangeNBiFunc::t_args_beg()
{
    return t_args.first;
}

CarCdrIterator RangeNBiFunc::t_args_end()
{
    return t_args.second;
}

CarCdrConstIterator RangeNBiFunc::t_args_beg() const
{
    return t_args.first;
}

CarCdrConstIterator RangeNBiFunc::t_args_end() const
{
    return t_args.second;
}

bool RangeNBiFunc::t_eval_args()
{
    return true;
}

stages RangeNBiFunc::t_internal_execute()
{
    return t_execute_func();
}
