#include "Func.h"
#include "CoreEnv.h"
#include "Bifuncs.h"
#include "SupportFuncs.h"
#include "CoreEnv.h"
#include "Funcs.h"

#include "common_funcs.h"
#include <array>
using namespace std;

namespace Func_transitions {

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

        return result;
    }();


    constexpr auto type = []() {
        auto result = make_array<bool, unsigned(stages::unkwnown)>(false);

        result[unsigned(stages::executed)] = true;
        result[unsigned(stages::args_eval)] = true;
        result[unsigned(stages::cycle_before_args_eval)] = true;
        result[unsigned(stages::before_args_eval)] = true;
        result[unsigned(stages::cycle_args_eval)] = true;
        result[unsigned(stages::after_args_eval)] = true;
        result[unsigned(stages::cycle_after_args_eval)] = true;
        result[unsigned(stages::execution)] = true;
        result[unsigned(stages::cycle_execution)] = true;

        return result;
    }();
}

Func::Func(func_id id, bool skip_eval_args):
    t_id(id),
    t_stage((skip_eval_args) ? stages::execution : stages::before_args_eval),
    t_bufs()
{
}

Func::~Func()
{
    if (Func_transitions::type[unsigned(t_stage)]) {
        t_bufs.cell.~Cell();
    }
    else {
        t_bufs.next.~FuncHolder();
    }
}

Func& Func::operator=(Func&& rh) noexcept
{
    if (this != &rh) {
        if (Func_transitions::type[unsigned(t_stage)]) {
            if (Func_transitions::type[unsigned(rh.t_stage)]) {
                t_bufs.cell = std::move(rh.t_bufs.cell);
            }
            else {
                t_bufs.cell.~Cell();
                new (&t_bufs.next) CoreData::HolderPtr(move(rh.t_bufs.next));
            }
        }
        else {
            if (Func_transitions::type[unsigned(rh.t_stage)]) {
                t_bufs.next.~FuncHolder();
                new (&t_bufs.cell) Cell(move(rh.t_bufs.cell));
            }
            else {
                t_bufs.next = std::move(rh.t_bufs.next);
            }
        }
        t_stage = rh.t_stage;
        t_id = rh.t_id;
    }
    return *this;
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
    t_bufs.cell.~Cell();
    new (&t_bufs.next) CoreData::HolderPtr(move(next));
}

void Func::f_eval_next(CoreData::HolderPtr&& next)
{
    t_stage = Func_transitions::f_eval_next[unsigned(t_stage)];
    t_bufs.cell.~Cell();
    new (&t_bufs.next) CoreData::HolderPtr(move(next));
}

void Func::f_eval_next_and_next(CoreData::HolderPtr&& next)
{
    t_stage = Func_transitions::f_eval_next_and_next[unsigned(t_stage)];
    t_bufs.cell.~Cell();
    new (&t_bufs.next) CoreData::HolderPtr(move(next));
}

void Func::f_cycle()
{
    t_stage = Func_transitions::f_cycle[unsigned(t_stage)];
}

void Func::f_return(const Cell& result)
{
    t_stage = Func_transitions::f_return[unsigned(t_stage)];
    t_bufs.cell = result;
}

void Func::f_push_next(const Cell& result)
{
    t_stage = Func_transitions::f_push_next[unsigned(t_stage)];
    t_bufs.next.~FuncHolder();
    new (&t_bufs.cell) Cell(result);
}

void Func::f_next()
{
    t_stage = Func_transitions::f_next[unsigned(t_stage)];
}

Cell Func::s_last_eval_val()
{
    switch (t_stage)
    {
    case stages::before_args_eval:
    case stages::args_eval:
    case stages::after_args_eval:
    case stages::execution:
        return t_bufs.cell;
    default:
        throw "Func::s_last_eval_val: wrong stage";
    }
}

Cell Func::s_result()
{
    switch (t_stage)
    {
    case stages::executed:
        return std::move(t_bufs.cell);
    default:
        throw "Func::s_result: wrong stage";
    }
}

CoreData::HolderPtr Func::s_get_next()
{
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
        return move(t_bufs.next);
    default:
        throw "Func::s_get_next: wrong stage";
    }
}