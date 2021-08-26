#include "AFuncs.h"
#include "CoreEnv.h"
#include "Bifuncs.h"
#include "SupportFuncs.h"

using namespace std;

Func::Func(CoreEnvironment& env):
    t_env_data(env),
    t_next_func_buf(nullptr, nullptr)
{
}

bool Func::execute()
{
    switch (t_stage)
    {
    case stages::executed:
        throw "";
        break;
    case stages::need_external:
        if (t_eval_next_flag) {
            t_eval_next_flag = false;
            t_stage = (!t_eval_args_flag) ? stages::not_args_evaled : stages::intermediate;
        }
        else {
            t_stage = stages::executed;
            return true;
        }
        [[fallthrough]];
    default:
        for(;;) {
            switch (t_stage) {
            case stages::not_started:
                t_init_before_args();
                if (t_stage == stages::not_started)t_stage = stages::not_args_evaled;
                else break;
                [[fallthrough]];
            case stages::not_args_evaled:
                while (t_stage == stages::not_args_evaled && !t_eval_args_flag) {
                    t_eval_args_flag = t_eval_args();
                }
                if (t_stage == stages::not_args_evaled && t_eval_args_flag) t_stage = stages::not_inited;
                else break;
                [[fallthrough]];
            case stages::not_inited:
                t_init_after_args();
                if (t_stage == stages::not_inited) t_stage = stages::intermediate;
                else break;
                [[fallthrough]];
            case stages::intermediate:
                while (t_stage == stages::intermediate) {
                    t_internal_execute();
                }
                break;
            case stages::executed:
                return true;
            case stages::need_external:
                return false;
            default:
                throw "";
                break;
            }
        }
    }
}

void Func::push_next(Cell&& result)
{
    if (t_stage != stages::need_external) throw "Func::push_next: wrong stage";
    t_buf = move(result);
}

CoreData::HolderPtr Func::get_next()
{
    if (t_stage != stages::need_external || !t_next_func_buf) throw "";
    return move(t_next_func_buf);
}

Cell Func::result() 
{
    if (empty(t_buf)) throw "";
    return move(t_buf);
}

void Func::t_init_before_args()
{
}

void Func::t_init_after_args()
{
}

CoreEnvironment& Func::t_env()
{
    return t_env_data;
}

const CoreEnvironment& Func::t_env() const
{
    return t_env_data;
}

SExprsFarm& Func::t_farm()
{
    return t_env_data.get().t_farm;
}

FuncsStorage& Func::t_funcs()
{
    return t_env_data.get().t_funcs;
}

CellEnvironment& Func::t_envs()
{
    return t_env_data.get().t_envs;
}

Syntaxer& Func::t_syntaxer()
{
    return t_env_data.get().t_syntaxer;
}

OutputController& Func::t_output_control()
{
    return t_env_data.get().t_output_control;
}

support_funcs& Func::t_sup_funcs()
{
    return t_env_data.get().t_support;
}

void Func::t_return(const Cell& result)
{
    t_buf = result;
    t_stage = stages::executed;
}

void Func::t_return_next(CoreData::HolderPtr&& next)
{
    t_next_func_buf = move(next);
    t_stage = stages::need_external;
}

void Func::t_eval_next(CoreData::HolderPtr&& next)
{
    t_eval_next_flag = true;
    t_next_func_buf = move(next);
    t_stage = stages::need_external;
}

void Func::t_cycle()
{
}

Cell Func::t_last_eval_val()
{
    if (empty(t_buf)) throw "";
    return t_buf;
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

void RangeBiFunc::t_internal_execute()
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

void RangeNBiFunc::t_internal_execute()
{
    return t_execute_func();
}
