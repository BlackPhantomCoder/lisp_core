#include "Func.h"
#include "CoreEnv.h"
#include "Bifuncs.h"
#include "SupportFuncs.h"
#include "CoreEnv.h"
#include "Funcs.h"

#include <array>
using namespace std;

namespace Func_transitions {

    //make array filled with value
    template<typename T, size_t N>
    constexpr auto make_array(T value)
    {
        auto a = std::array<T, N>{};
        for (auto& x : a)
            x = value;
        return a;
    }

    constexpr auto f_next = []() {
        auto result = make_array<stages, unsigned(stages::unkwnown)>(stages::unkwnown);

        result[unsigned(stages::before_args_eval)] = stages::args_eval;
        result[unsigned(stages::args_eval)] = stages::after_args_eval;
        result[unsigned(stages::after_args_eval)] = stages::execution;
        result[unsigned(stages::cycle_before_args_eval)] = stages::before_args_eval;
        result[unsigned(stages::cycle_args_eval)] = stages::args_eval;
        result[unsigned(stages::cycle_after_args_eval)] = stages::after_args_eval;
        result[unsigned(stages::cycle_execution)] = stages::execution;

        result[unsigned(stages::need_external_before_args_eval)] = stages::need_external_before_args_eval;
        result[unsigned(stages::need_external_args_eval)] = stages::need_external_args_eval;
        result[unsigned(stages::need_external_after_args_eval)] = stages::need_external_after_args_eval;
        result[unsigned(stages::need_external_before_args_eval_plus_next)] = stages::need_external_before_args_eval_plus_next;
        result[unsigned(stages::need_external_args_eval_plus_next)] = stages::need_external_args_eval_plus_next;
        result[unsigned(stages::need_external_after_args_eval_plus_next)] = stages::need_external_after_args_eval_plus_next;
        result[unsigned(stages::need_external_execution)] = stages::need_external_execution;
        result[unsigned(stages::need_external_return_next)] = stages::need_external_return_next;
        result[unsigned(stages::executed)] = stages::executed;

        return result;
    }();

    constexpr auto f_return_next = []() {
        auto result = make_array<stages, unsigned(stages::unkwnown)>(stages::unkwnown);

        result[unsigned(stages::before_args_eval)] = stages::need_external_return_next;
        result[unsigned(stages::args_eval)] = stages::need_external_return_next;
        result[unsigned(stages::after_args_eval)] = stages::need_external_return_next;
        result[unsigned(stages::execution)] = stages::need_external_return_next;

        return result;
    }();

    constexpr auto f_eval_next = []() {
        auto result = make_array<stages, unsigned(stages::unkwnown)>(stages::unkwnown);

        result[unsigned(stages::before_args_eval)] = stages::need_external_before_args_eval;
        result[unsigned(stages::args_eval)] = stages::need_external_args_eval;
        result[unsigned(stages::after_args_eval)] = stages::need_external_after_args_eval;
        result[unsigned(stages::execution)] = stages::need_external_execution;

        return result;
    }();

    constexpr auto f_eval_next_and_next = []() {
        auto result = make_array<stages, unsigned(stages::unkwnown)>(stages::unkwnown);

        result[unsigned(stages::before_args_eval)] = stages::need_external_before_args_eval_plus_next;
        result[unsigned(stages::args_eval)] = stages::need_external_args_eval_plus_next;
        result[unsigned(stages::after_args_eval)] = stages::need_external_after_args_eval_plus_next;

        return result;
    }();

    constexpr auto f_cycle = []() {
        auto result = make_array<stages, unsigned(stages::unkwnown)>(stages::unkwnown);

        result[unsigned(stages::before_args_eval)] = stages::cycle_before_args_eval;
        result[unsigned(stages::args_eval)] = stages::cycle_args_eval;
        result[unsigned(stages::after_args_eval)] = stages::cycle_after_args_eval;
        result[unsigned(stages::execution)] = stages::cycle_execution;

        return result;
    }();

    constexpr auto f_return = []() {
        auto result = make_array<stages, unsigned(stages::unkwnown)>(stages::unkwnown);

        result[unsigned(stages::before_args_eval)] = stages::executed;
        result[unsigned(stages::args_eval)] = stages::executed;
        result[unsigned(stages::after_args_eval)] = stages::executed;
        result[unsigned(stages::execution)] = stages::executed;

        return result;
    }();

    constexpr auto f_push_next = []() {
        auto result = make_array<stages, unsigned(stages::unkwnown)>(stages::unkwnown);

        result[unsigned(stages::need_external_before_args_eval)] = stages::before_args_eval;
        result[unsigned(stages::need_external_args_eval)] = stages::args_eval;
        result[unsigned(stages::need_external_after_args_eval)] = stages::after_args_eval;
        result[unsigned(stages::need_external_before_args_eval_plus_next)] = stages::args_eval;
        result[unsigned(stages::need_external_args_eval_plus_next)] = stages::after_args_eval;
        result[unsigned(stages::need_external_after_args_eval_plus_next)] = stages::execution;
        result[unsigned(stages::need_external_execution)] = stages::execution;
        result[unsigned(stages::need_external_return_next)] = stages::executed;

        return result;
    }();


    
}

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
    t_stage = Func_transitions::f_return_next[unsigned(t_stage)];
    t_bufs = move(next);
}

void Func::f_eval_next(CoreData::HolderPtr&& next)
{
    t_stage = Func_transitions::f_eval_next[unsigned(t_stage)];
    t_bufs = move(next);
}

void Func::f_eval_next_and_next(CoreData::HolderPtr&& next)
{
    t_stage = Func_transitions::f_eval_next_and_next[unsigned(t_stage)];
    t_bufs = move(next);
}

void Func::f_cycle()
{
    t_stage = Func_transitions::f_cycle[unsigned(t_stage)];
}

void Func::f_return(const Cell& result)
{
    t_stage = Func_transitions::f_return[unsigned(t_stage)];
    t_bufs = result;
}

void Func::f_push_next(const Cell& result)
{
    t_stage = Func_transitions::f_push_next[unsigned(t_stage)];
    t_bufs = result;
}

void Func::f_next()
{
    t_stage = Func_transitions::f_next[unsigned(t_stage)];
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
