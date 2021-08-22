#include "SupportFuncs.h"
#include "Bifuncs.h"
#include "CoreEnv.h"
#include "Funcs.h"
#include "Symbol.h"
#include "DotPair.h"
#include "Number.h"
#include "SupportFuncs.h"
#include "CoreEnv.h"
#include "ArgsCounter.h"

#include <iostream>
#include <algorithm>
#include <sstream>
using namespace std;
using namespace CoreData;

#define arg1 *t_args_beg()
#define arg2 *(next(t_args_beg()))
#define arg3 *(next(t_args_beg()), 2)

EvalLambda::EvalLambda(
    CoreEnvironment& env,
    const lambda& l,
    CarCdrIterator beg_it,
    CarCdrIterator end_it,
    bool forse_noeval_args
):
    Func(env),
    t_l(l)
{
    if (l.type == lambda_types::nlambda || forse_noeval_args) {
        t_args = pair{ beg_it , end_it };
    }
    else {
        t_args_eval_func = make_fnc<EvalQuoteRange>(t_env(), beg_it, end_it);
    }
    t_costil_param = { t_l.get().params };
    t_costil_body = { t_l.get().body };
}

void EvalLambda::t_init_before_args()
{
    if (t_args_eval_func) {
        t_eval_next(move(*t_args_eval_func));
        t_args_eval_func.reset();
    }
    return t_cycle();
}

void EvalLambda::t_init_after_args()
{
    return t_eval_next(
        make_fnc<ProgN>(
            t_env(),
            begin(t_costil_body),
            end(t_costil_body)
        )
    );
}

bool EvalLambda::t_eval_args()
{
    if(holds_alternative<monostate>(t_args)) t_args = t_last_eval_val();
    t_env().t_envs.push(t_create_frame());
    return true;
}

void EvalLambda::t_internal_execute()
{
    t_env().t_envs.pop();
    return t_return(t_last_eval_val());
}

CellEnvironment::frame EvalLambda::t_create_frame()
{
    if (holds_alternative<monostate>(t_args)) throw "";
    CellEnvironment::frame buf;
    if (t_l.get().arg_type == lambda_args_types::spread) {
        CarCdrIterator arg_it;
        CarCdrIterator arg_end_it;
        if (holds_alternative<Cell>(t_args)) {
            arg_it = begin(get<Cell>(t_args));
            arg_end_it = end(get<Cell>(t_args));
        }
        else {
            arg_it = get<pair<CarCdrIterator, CarCdrIterator>>(t_args).first;
            arg_end_it = get<pair<CarCdrIterator, CarCdrIterator>>(t_args).second;
        }
        buf.first = { begin(t_costil_param), end(t_costil_param) };
        buf.second = { arg_it, arg_end_it };
    }
    else {
        if (holds_alternative<Cell>(t_args)) {
            t_temp = t_farm().make_list_cell({ get<Cell>(t_args) });
            buf.first = { begin(t_costil_param), end(t_costil_param) };
            buf.second = { begin(t_temp), end(t_temp) };
        }
        else {
            t_temp = t_farm().make_list_cell(
                get<pair<CarCdrIterator, CarCdrIterator>>(t_args).first,
                get<pair<CarCdrIterator, CarCdrIterator>>(t_args).second
            );
            buf.first = { begin(t_costil_param), end(t_costil_param) };
            buf.second = { begin(t_temp), end(t_temp) };
        }
      
    }
    return buf;
}


EvalQuote::EvalQuote(CoreEnvironment& env, Cell& c) :
    Func(env),
    t_arg(&c)
{
}

bool EvalQuote::t_eval_args()
{
    return true;
}

void EvalQuote::t_internal_execute()
{
    if (is_list(*t_arg)) {
        if (is_null_list(to_list(*t_arg))) return t_return(t_farm().nil());
        if (auto& cdr_buf = cdr(*t_arg); is_list(cdr_buf)) {
            return t_eval_func(car(*t_arg), begin(cdr_buf), end(cdr_buf));
        }
        else {
            //if args is dotpair
            return t_eval_func(car(*t_arg), CarCdrIterator(), CarCdrIterator());
        }
    }
    else {
        return t_return(
            t_sup_funcs().eval_atom(*t_arg)
        );
    }
}

void EvalQuote::t_eval_func(Cell& fnc, CarCdrIterator args_beg_it, CarCdrIterator args_end_it)
{
    return t_return_next(make_fnc<EvalFunc>(t_env(), fnc, args_beg_it, args_end_it));
}


EvalQuoteRange::EvalQuoteRange(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it) :
    RangeNBiFunc(env, args_beg_it, args_end_it)
{
}

void EvalQuoteRange::t_init_after_args()
{
    t_it = t_args_beg();
    if (t_args_beg() == t_args_end()) {
        return t_return(t_farm().make_empty_list_cell());
    }

    return t_to_next();
}

void EvalQuoteRange::t_execute_func()
{
    if (t_it != t_args_end()) {
        t_result_v.push_back(t_last_eval_val());
        return t_to_next();
    }
    if (!t_last)t_result_v.push_back(t_last_eval_val());
    return t_return(t_farm().make_list_cell(begin(t_result_v), end(t_result_v)));
}

void EvalQuoteRange::t_to_next()
{
    std::reference_wrapper<Cell> c = *t_it++;
    while (t_it != t_args_end()) {
        if (!is_list(c)) {
            t_result_v.push_back(t_sup_funcs().eval_atom(c));
        }
        else if (is_null_list(to_list(c))) {
            t_result_v.push_back(c);
        }
        else {
            break;
        }
        c = *t_it++;
    }
    if (is_list(c)) {
        if (is_null_list(to_list(c))) {
            t_result_v.push_back(c);
        }
        else {
            return t_eval_fnc(c);
        }
    }
    else {
        t_result_v.push_back(t_sup_funcs().eval_atom(c));
    }
    t_last = t_it == t_args_end();
    return t_cycle();
}

void EvalQuoteRange::t_eval_fnc(Cell lst)
{
    if (auto& cdr_buf = cdr(lst); is_list(cdr_buf)) {
        return t_eval_next(make_fnc<EvalFunc>(t_env(), car(lst), begin(cdr_buf), end(cdr_buf)));
    }
    else {
        //if args is dotpair
        return t_eval_next(make_fnc<EvalFunc>(t_env(), car(lst), CarCdrIterator(), CarCdrIterator()));
    }
}


ImplicitCond::ImplicitCond(CoreEnvironment& env, Cell& atom) :
    Func(env),
    t_atom(&atom)
{
}

void ImplicitCond::t_init_after_args()
{
    return t_eval_next(
        make_fnc<EvalQuote>(
            t_env(),
            car(*t_atom)
        )
    );
}

bool ImplicitCond::t_eval_args()
{
    return true;
}

void ImplicitCond::t_internal_execute()
{
    if (t_next_res) {
        return t_return(t_farm().make_list_cell(t_last_eval_val()));
    }

    if (empty(t_predicate_val_buf)) {
        t_predicate_val_buf = t_last_eval_val();
    }
    //cout << "cond " << t_env.t_output_control.to_string(t_atom) << endl;
    //cout << "cond " << t_env.t_output_control.to_string(*t_predicate_val_buf) << endl;

    if (!is_null(t_predicate_val_buf)) {
        //cout << "cond " << t_env.t_output_control.to_string(cdr(t_atom)) << endl;
        auto& cdr_buf = cdr(*t_atom);
        if (!is_list(cdr_buf) || is_null(cdr_buf))
        {
            return t_return(t_farm().make_list_cell(t_predicate_val_buf));
        }
        t_next_res = true;
        return t_eval_next(
            make_fnc<ProgN>(
                t_env(),
                begin(cdr_buf),
                end(cdr_buf)
            )
        );
    }

    return t_return(t_farm().nil());
}


EvalFunc::EvalFunc(CoreEnvironment& env, Cell& fnc, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval) :
    Func(env),
    t_args_beg(args_beg_it),
    t_args_end(args_end_it),
    t_fnc(&fnc),
    t_forse_noeval_func(forse_noeval)
{
}

bool EvalFunc::t_eval_args()
{
    return true;
}

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

void EvalFunc::t_internal_execute()
{
    // direct call bifunc
    if (t_bifunc) {
        auto c = t_last_eval_val();
        return t_return(((&t_env())->*(t_bifunc))(begin(c), end(c)));
    }

    if (is_lambda_form(*t_fnc, t_farm())) {
        t_buf = t_sup_funcs().make_lambda_form(*t_fnc);
        return t_return_next(
            make_fnc<EvalLambda>(
                t_env(),
                *t_buf,
                t_args_beg,
                t_args_end,
                t_forse_noeval_func
            )
        );
    }

    if (!is_symbol(*t_fnc)) throw string("eval_fnc: unknown function ") + t_output_control().to_string(*t_fnc);
    if (auto fnc_r = t_funcs().find(to_symbol(*t_fnc))) {
        return std::visit(overloaded{
                [this](const FuncsStorage::bifunc& arg) {
                    t_bifunc = arg.ptr;
                    return t_eval_next(make_fnc<EvalQuoteRange>(t_env(), t_args_beg, t_args_end));
                },
                [this](const FuncsStorage::nbifunc& arg) {
                    return t_return(((&t_env())->*(arg.ptr))(t_args_beg, t_args_end));
                },
                [this](const lambda& arg) {
                    return t_return_next(
                       make_fnc<EvalLambda>(
                           t_env(),
                           arg,
                           t_args_beg,
                           t_args_end,
                           t_forse_noeval_func
                           )
                    );
                },
                [this](const CoreData::special_bifunc_make& arg) {
                    return t_return_next(arg(t_env(), t_args_beg, t_args_end, t_forse_noeval_func));
                },
                [this](const macro& arg) {
                    return t_return_next(make_fnc<EvalMacro>(t_env(),arg, t_args_beg, t_args_end, t_forse_noeval_func));
                },
                [this](const CoreData::special_nbifunc_make& arg) {
                     return t_return_next(arg(t_env(), t_args_beg, t_args_end));
                }

            }, fnc_r->get());
    }
    else {
        throw "eval_fnc error " + t_output_control().to_string(
            t_farm().make_list_cell({ *t_fnc, t_farm().make_list_cell(t_args_beg , t_args_end) })
        );
    }
}

ExpandMacro::ExpandMacro(
    CoreEnvironment& env,
    const macro& macro,
    CarCdrIterator args_beg_it,
    CarCdrIterator args_end_it,
    bool forse_noeval,
    bool once
) :
    RangeNBiFunc(env, args_beg_it, args_end_it),
    t_macro(&macro),
    t_forse_noeval(forse_noeval),
    t_once(once)
{
}

void ExpandMacro::t_init_after_args()
{
    if (t_args_beg() == t_args_end()) return t_return(t_farm().nil());
    t_param_list_b = t_args_beg();
    t_param_list_e = t_args_end();
}

void ExpandMacro::t_execute_func()
{
    if (t_buf_flag) {
        t_buf = t_last_eval_val();
        if (t_once) return t_return(t_buf);
        return t_init_from_buf();
    }
    t_buf_flag = true;
    t_args = link_params(*t_macro, t_param_list_b, t_param_list_e, t_farm());
    return t_eval_next(
        make_fnc<EvalLambda>(
            t_env(),
            t_macro->body,
            begin(t_args),
            end(t_args),
            t_forse_noeval
        )
    );
}

void ExpandMacro::t_init_from_buf()
{
    if (auto m_opt = is_macro_call(t_buf, t_env())) {
        t_macro = &m_opt->get();
        auto& c = cdr(t_buf);
        t_param_list = c;
        t_param_list_b = begin(t_param_list);
        t_param_list_e = end(t_param_list);
        t_buf_flag = false;
        return t_cycle();
    }

    return t_return(t_buf);
}


EvalMacro::EvalMacro(
    CoreEnvironment& env,
    const macro& macro,
    CarCdrIterator args_beg_it,
    CarCdrIterator args_end_it,
    bool forse_noeval
) :
    Func(env),
    t_fnc( make_fnc<ExpandMacro>(env, macro, args_beg_it, args_end_it, forse_noeval))
{
}

bool EvalMacro::t_eval_args()
{
    if (t_ev) {
        t_buf = t_last_eval_val();
        return true;
    }
    t_eval_next(move(t_fnc));
    t_ev = true;
    return false;
}

void EvalMacro::t_internal_execute()
{
    //cout << t_env().t_output_control.to_string(t_buf) << endl;
    return t_return_next(make_fnc<EvalQuote>(t_env(), t_buf));
}