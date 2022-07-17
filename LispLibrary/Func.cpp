#include "Func.h"
#include "CoreEnv.h"
#include "Bifuncs.h"
#include "SupportFuncs.h"
#include "CoreEnv.h"
#include "Funcs.h"

using namespace std;

Func::Func(func_id id, bool skip_eval_args):
    t_id(id),
    t_stage((skip_eval_args) ? stages::execution : stages::before_args_eval)
{
}

stages Func::stage() const
{
    return t_stage;
}

func_id Func::id() const
{
    return t_id;
}

void Func::f_return_next(CoreData::HolderPtr&& next)
{
    switch (t_stage)
    {
    case stages::before_args_eval:
    case stages::args_eval:
    case stages::after_args_eval:
    case stages::execution:
        t_stage = stages::need_external_return_next;
        t_bufs = move(next);
        break;
    default:
        throw "Func::f_return_next: wrong stage";
    }
}

void Func::f_eval_next(CoreData::HolderPtr&& next)
{
    switch (t_stage)
    {
    case stages::before_args_eval:
        t_stage = stages::need_external_before_args_eval;
        break;
    case stages::args_eval:
        t_stage = stages::need_external_args_eval;
        break;
    case stages::after_args_eval:
        t_stage = stages::need_external_args_eval;
        break;
    case stages::execution:
        t_stage = stages::need_external_execution;
        break;
    default:
        throw "Func::f_eval_next: wrong stage";
    }
    t_bufs = move(next);
}

void Func::f_eval_next_and_next(CoreData::HolderPtr&& next)
{
    switch (t_stage)
    {
    case stages::before_args_eval:
        t_stage = stages::need_external_before_args_eval_plus_next;
        break;
    case stages::args_eval:
        t_stage = stages::need_external_args_eval_plus_next;
        break;
    case stages::after_args_eval:
        t_stage = stages::need_external_after_args_eval_plus_next;
        break;
    default:
        throw "Func::f_eval_next_and_next: wrong stage";
    }
    t_bufs = move(next);
}

void Func::f_cycle()
{

    switch (t_stage)
    {
    case stages::before_args_eval:
        t_stage = stages::cycle_before_args_eval;
        break;
    case stages::args_eval:
        t_stage = stages::cycle_args_eval;
        break;
    case stages::after_args_eval:
        t_stage = stages::cycle_after_args_eval;
        break;
    case stages::execution:
        t_stage = stages::cycle_execution;
        break;
    default:
        throw "Func::f_cycle: wrong stage";
    }
}

void Func::f_return(const Cell& result)
{
    switch (t_stage)
    {
    case stages::before_args_eval:
    case stages::args_eval:
    case stages::after_args_eval:
    case stages::execution:
        t_bufs = result;
        t_stage = stages::executed;
        break;
    default:
        throw "Func::f_return: wrong stage";
    }
}

void Func::f_push_next(const Cell& result)
{
    switch (t_stage)
    {
    case stages::need_external_before_args_eval:
        t_stage = stages::before_args_eval;
        break;
    case stages::need_external_args_eval:
        t_stage = stages::args_eval;
        break;
    case stages::need_external_after_args_eval:
        t_stage = stages::after_args_eval;
        break;
    case stages::need_external_before_args_eval_plus_next:
        t_stage = stages::args_eval;
        break;
    case stages::need_external_args_eval_plus_next:
        t_stage = stages::after_args_eval;
        break;
    case stages::need_external_after_args_eval_plus_next:
        t_stage = stages::execution;
        break;
    case stages::need_external_execution:
        t_stage = stages::execution;
        break;
    case stages::need_external_return_next:
        t_stage = stages::executed;
        break;
    default:
        throw "Func::f_push_next: wrong stage";
    }
    t_bufs = result;
}

void Func::f_next()
{

    switch (t_stage)
    {
    case stages::before_args_eval:
        t_stage = stages::args_eval;
        break;
    case stages::args_eval:
        t_stage = stages::after_args_eval;
        break;
    case stages::after_args_eval:
        t_stage = stages::execution;
        break;
    case stages::cycle_before_args_eval :
        t_stage = stages::before_args_eval;
        break;
    case stages::cycle_args_eval :
        t_stage = stages::args_eval;
        break;
    case stages::cycle_after_args_eval:
        t_stage = stages::after_args_eval;
        break;
    case stages::cycle_execution :
        t_stage = stages::execution;
        break;
    case stages::need_external_before_args_eval:
    case stages::need_external_args_eval:
    case stages::need_external_after_args_eval:
    case stages::need_external_before_args_eval_plus_next:
    case stages::need_external_args_eval_plus_next:
    case stages::need_external_after_args_eval_plus_next:
    case stages::need_external_execution:
    case stages::need_external_return_next:
    case stages::executed:
        break;
    default:
        throw "Func::f_next: wrong stage";
    }
}

Cell Func::s_last_eval_val()
{
    if (!holds_alternative<Cell>(t_bufs)) throw "Func::s_last_eval_val: empty";
    switch (t_stage)
    {
    case stages::before_args_eval:
    case stages::args_eval:
    case stages::after_args_eval:
    case stages::execution:
        return get<Cell>(t_bufs);
    default:
        throw "Func::s_last_eval_val: wrong stage";
    }
}

Cell Func::s_result()
{
    if (!holds_alternative<Cell>(t_bufs)) throw "Func::s_result: empty";
    switch (t_stage)
    {
    case stages::executed:
        return get<Cell>(t_bufs);
    default:
        throw "Func::s_result: wrong stage";
    }
}

CoreData::HolderPtr Func::s_get_next()
{
    if (!holds_alternative<CoreData::HolderPtr>(t_bufs)) throw "Func::s_get_next: empty";
    switch (t_stage)
    {
    case stages::need_external_before_args_eval:
    case stages::need_external_args_eval:
    case stages::need_external_after_args_eval:
    case stages::need_external_before_args_eval_plus_next:
    case stages::need_external_args_eval_plus_next:
    case stages::need_external_after_args_eval_plus_next:
    case stages::need_external_execution:
    case stages::need_external_return_next:
        return move(get<CoreData::HolderPtr>(t_bufs));
    default:
        throw "Func::s_get_next: wrong stage";
    }
}
