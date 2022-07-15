#include "Func.h"
#include "CoreEnv.h"
#include "Bifuncs.h"
#include "SupportFuncs.h"
#include "CoreEnv.h"
#include "Funcs.h"

using namespace std;

Func::Func(CoreEnvironment& env):
    t_env_data(&env)
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
    t_bufs = move(result);
}

CoreData::HolderPtr Func::get_next()
{
    if (t_stage != stages::need_external || !holds_alternative<CoreData::HolderPtr>(t_bufs)) throw "";
    return move(get<CoreData::HolderPtr>(t_bufs));
}

Cell Func::result() 
{
    if (!holds_alternative<Cell>(t_bufs)) throw "";
    return move(get<Cell>(t_bufs));
}

void Func::t_init_before_args()
{
}

void Func::t_init_after_args()
{
}

CoreEnvironment& Func::t_env()
{
    return *t_env_data;
}

const CoreEnvironment& Func::t_env() const
{
    return *t_env_data;
}

void Func::t_return(const Cell& result)
{
    t_bufs = result;
    t_stage = stages::executed;
}

void Func::t_return_next(CoreData::HolderPtr&& next)
{
    t_bufs = move(next);
    t_stage = stages::need_external;
}

void Func::t_eval_next(CoreData::HolderPtr&& next)
{
    t_eval_next_flag = true;
    t_bufs = move(next);
    t_stage = stages::need_external;
}

void Func::t_cycle()
{
}

Cell Func::t_last_eval_val()
{
    if(!holds_alternative<Cell>(t_bufs)) throw "";
    return get<Cell>(t_bufs);
}
