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

#define arg1 *args_beg()
#define arg2 *(next(args_beg()))
#define arg3 *(next(args_beg()), 2)

EvalLambda::EvalLambda(
    const lambda& l,
    CarCdrIterator beg_it,
    CarCdrIterator end_it,
    bool forse_noeval_args
):
    Func(func_id::evallambda, true),
    t_arg_type(l.arg_type)
{
    if (l.type == lambda_types::nlambda || forse_noeval_args) {
        t_args = pair{ beg_it , end_it };
        t_stage = stage::main_noeval;
    }
    else {
        t_args = make_fnc<EvalQuoteRange>(beg_it, end_it);
        t_stage = stage::args_eval;
    }
    t_costil_param = { l.params };
    t_costil_body = { l.body };
}

void EvalLambda::execute(CoreEnvironment& env)
{
    switch (t_stage)
    {
    case EvalLambda::stage::args_eval:
        t_stage = stage::args_evaled;
        return f_eval_next(move(get<CoreData::HolderPtr>(t_args)));
    case EvalLambda::stage::args_evaled:
        t_args = s_last_eval_val();
        [[fallthrough]];
    case EvalLambda::stage::main:
        t_stage = stage::evaled;
        {
            auto buf = CellEnvironment::frame{};
            if (t_arg_type == lambda_args_types::spread) {
                buf.first = { begin(t_costil_param), end(t_costil_param) };
                buf.second = { begin(get<Cell>(t_args)), end(get<Cell>(t_args)) };
            }
            else {
                t_temp = env.farm().make_list_cell({ get<Cell>(t_args) });
                buf.first = { begin(t_costil_param), end(t_costil_param) };
                buf.second = { begin(t_temp), end(t_temp) };
            }
            env.envs().push(move(buf));
        }
        return f_eval_next(make_fnc<ProgN>(begin(t_costil_body), end(t_costil_body)));
    case EvalLambda::stage::main_noeval:
        t_stage = stage::evaled;
        {
            auto buf = CellEnvironment::frame{};
            if (t_arg_type == lambda_args_types::spread) {
                buf.first = { begin(t_costil_param), end(t_costil_param) };
                buf.second = { 
                    get<pair<CarCdrIterator, CarCdrIterator>>(t_args).first,
                    get<pair<CarCdrIterator, CarCdrIterator>>(t_args).second
                };
            }
            else {
                t_temp = env.farm().make_list_cell(env.farm().make_list_cell(
                    get<pair<CarCdrIterator, CarCdrIterator>>(t_args).first,
                    get<pair<CarCdrIterator, CarCdrIterator>>(t_args).second
                ));
                buf.first = { begin(t_costil_param), end(t_costil_param) };
                buf.second = { begin(t_temp), end(t_temp) };
            }
            env.envs().push(move(buf));
        }
        return f_eval_next(make_fnc<ProgN>(begin(t_costil_body), end(t_costil_body)));
    case EvalLambda::stage::evaled:
        env.envs().pop();
        return f_return(s_last_eval_val());
    default:
        throw logic_error("EvalLambda::execute: unknown stage");
    }

}

EvalQuote::EvalQuote(Cell& c) :
    Func(func_id::evalquote, true),
    t_arg(&c)
{
}


void EvalQuote::execute(CoreEnvironment& env)
{
    if (is_list(*t_arg)) {
        if (is_null_list(to_list(*t_arg))) return f_return(env.farm().nil());
        if (auto& cdr_buf = cdr(*t_arg); is_list(cdr_buf)) {
            return t_eval_func(car(*t_arg), begin(cdr_buf), end(cdr_buf));
        }
        else {
            //if args is dotpair
            return t_eval_func(car(*t_arg), CarCdrIterator(), CarCdrIterator());
        }
    }
    else {
        return f_return(
            env.core_funcs().eval_atom(*t_arg)
        );
    }
}

void EvalQuote::t_eval_func(Cell& fnc, CarCdrIterator args_beg_it, CarCdrIterator args_end_it)
{
    return f_return_next(make_fnc<EvalFunc>(fnc, args_beg_it, args_end_it));
}

void EvalQuoteRange::execute(CoreEnvironment& env)
{
    switch (t_stage)
    {
    case EvalQuoteRange::stage::start:
        if (args_beg() == args_end()) {
            return f_return(env.farm().make_empty_list_cell());
        }
        t_it = args_beg();
        {
            auto& c = *t_it++;
            if (!is_list(c)) {
                t_push_back_first(env, env.core_funcs().eval_atom(c));
            }
            else if (is_null_list(to_list(c))) {
                t_push_back_first(env, c);
            }
            else {
                t_stage = stage::func_evaled_first;
                if (auto& cdr_buf = cdr(c); is_list(cdr_buf)) {
                    return f_eval_next(make_fnc<EvalFunc>(car(c), begin(cdr_buf), end(cdr_buf)));
                }
                else {
                    //if args is dotpair
                    return f_eval_next(make_fnc<EvalFunc>(car(c), CarCdrIterator(), CarCdrIterator()));
                }
            }
        }

        t_stage = stage::main;
        [[fallthrough]];
    case EvalQuoteRange::stage::main:
        //cycle because light work
        {
            auto cycles = 0;
            while (t_it != args_end()) {
                auto& c = *t_it++;
                if (!is_list(c)) {
                    t_push_back(env, env.core_funcs().eval_atom(c));
                }
                else if (is_null_list(to_list(c))) {
                    t_push_back(env, c);
                }
                else {
                    t_stage = stage::func_evaled;
                    if (auto& cdr_buf = cdr(c); is_list(cdr_buf)) {
                        return f_eval_next(make_fnc<EvalFunc>(car(c), begin(cdr_buf), end(cdr_buf)));
                    }
                    else {
                        //if args is dotpair
                        return f_eval_next(make_fnc<EvalFunc>(car(c), CarCdrIterator(), CarCdrIterator()));
                    }
                }
                //return f_cycle();
                ++cycles;
                if(cycles == 100) return f_cycle();
            }
        }
        
        return f_return(t_result);
    case EvalQuoteRange::stage::func_evaled:
        t_push_back(env, s_last_eval_val());
        t_stage = stage::main;
        return f_cycle();
    case EvalQuoteRange::stage::func_evaled_first:
        t_push_back_first(env, s_last_eval_val());
        t_stage = stage::main;
        return f_cycle();
    default:
        throw logic_error("EvalQuoteRange::execute: unknown stage");
    }
}

void EvalQuoteRange::t_push_back_first(CoreEnvironment& env, const Cell& c)
{
    t_result = env.farm().make_list_cell(c);
    t_result_last = &t_result;
}

void EvalQuoteRange::t_push_back(CoreEnvironment& env, const Cell& c)
{
    rplacd(to_list(*t_result_last), env.farm().make_list_cell(c));
    t_result_last = &cdr(*t_result_last);
}

ImplicitCond::ImplicitCond( Cell& atom) :
    Func(func_id::implicitcond, true),
    t_atom(&atom)
{
}

void ImplicitCond::execute(CoreEnvironment& env)
{
    switch (t_stage)
    {
    case ImplicitCond::stage::start:
        t_stage = stage::main;
        return f_eval_next(make_fnc<EvalQuote>(car(*t_atom)));
    case ImplicitCond::stage::main:
        //cout << "cond " << t_env.t_output_control.to_string(t_atom) << endl;
        //cout << "cond " << t_env.t_output_control.to_string(*t_predicate_val_buf) << endl;
        {
            auto predicate_val_buf = s_last_eval_val();
            if (!is_null(predicate_val_buf)) {
                //cout << "cond " << t_env.t_output_control.to_string(cdr(t_atom)) << endl;
                auto& cdr_buf = cdr(*t_atom);
                if (!is_list(cdr_buf) || is_null(cdr_buf))
                {
                    return f_return(env.farm().make_list_cell(predicate_val_buf));
                }
                t_stage = stage::res;
                return f_eval_next(
                    make_fnc<ProgN>(
                        begin(cdr_buf),
                        end(cdr_buf)
                    )
                );
            }
            return f_return(env.farm().nil());
        }
    case ImplicitCond::stage::res:
        return f_return(env.farm().make_list_cell(s_last_eval_val()));
    default:
        throw logic_error("ImplicitCond::execute: unknown stage");
    }
}


EvalFunc::EvalFunc(Cell& fnc, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval) :
    Func(func_id::evalfunc, true),
    t_args_beg(args_beg_it),
    t_args_end(args_end_it),
    //t_fnc(&fnc),
    t_buf(&fnc),
    t_forse_noeval_func(forse_noeval)
{
}


void EvalFunc::execute(CoreEnvironment& env)
{
    switch (t_buf.index())
    {
    case 0:
    {
        auto*& fnc = get<Cell*>(t_buf);
        if (is_lambda_form(*fnc, env.farm())) {
            return f_return_next(
                make_fnc<EvalLambda>(
                    env.core_funcs().make_lambda_form(*fnc),
                    t_args_beg,
                    t_args_end,
                    t_forse_noeval_func
                )
            );
        }

        if (!is_symbol(*fnc)) throw string("eval_fnc: unknown function ") + env.output_control().to_string(*fnc);
        //cout << "func: " << env.output_control().to_string(*fnc) << endl;
        if (auto fnc_r = env.funcs().find(to_symbol(*fnc))) {
            return std::visit([this, &env](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, FuncsStorage::bifunc>)
                {
                    if (t_args_beg == t_args_end) {
                        return f_return(arg.ptr(env, t_args_beg, t_args_end));
                    }
                    t_buf = arg.ptr;
                    return f_eval_next(make_fnc<EvalQuoteRange>(t_args_beg, t_args_end));
                }
                else if constexpr (std::is_same_v<T, FuncsStorage::nbifunc>)
                {
                    return f_return(arg.ptr(env, t_args_beg, t_args_end));
                }
                else if constexpr (std::is_same_v<T, lambda>)
                {
                    return f_return_next(
                        make_fnc<EvalLambda>(
                            arg,
                            t_args_beg,
                            t_args_end,
                            t_forse_noeval_func
                        )
                    );
                }
                else if constexpr (std::is_same_v<T, special_bifunc_make>)
                {
                    return f_return_next(arg(t_args_beg, t_args_end, t_forse_noeval_func));
                }
                else if constexpr (std::is_same_v<T, macro>)
                {
                    return f_return_next(make_fnc<EvalMacro>(arg, t_args_beg, t_args_end, t_forse_noeval_func));
                }
                else if constexpr (std::is_same_v<T, CoreData::special_nbifunc_make>)
                {
                    return f_return_next(arg(t_args_beg, t_args_end));
                }
            }, fnc_r->get());
        }
        else {
            throw "eval_fnc error " + env.output_control().to_string(
                env.farm().make_list_cell({ *fnc, env.farm().make_list_cell(t_args_beg , t_args_end) })
            );
        }
    }
    case 1:
    {
        // direct call bifunc
        auto c = s_last_eval_val();
        //cout << env.output_control().to_string(c) << endl;
        return f_return(get<CoreData::bifunc>(t_buf)(env, begin(c), end(c)));
    }
    default:
        throw logic_error("EvalFunc: execute");
    }
}

ExpandMacro::ExpandMacro(
    const macro& macro,
    CarCdrIterator args_beg_it,
    CarCdrIterator args_end_it,
    bool forse_noeval,
    bool once
) :
    RangeNBiFunc(func_id::expandmacro, args_beg_it, args_end_it),
    t_macro(&macro),
    t_forse_noeval(forse_noeval),
    t_stage((once) ? stage::once_start: stage::start)
{
}

void ExpandMacro::execute(CoreEnvironment& env)
{
    switch (t_stage)
    {
    case ExpandMacro::stage::once_start:
        if (args_beg() == args_end()) return f_return(env.farm().nil());
        t_buf = link_params(*t_macro, args_beg(), args_end(), env.farm());
        return f_return_next(
            make_fnc<EvalLambda>(
                t_macro->body,
                begin(t_buf),
                end(t_buf),
                t_forse_noeval
            )
        );
    case ExpandMacro::stage::start:
        {
            if (args_beg() == args_end()) return f_return(env.farm().nil());
            t_param_list_b = args_beg();
            t_param_list_e = args_end();
        }
        [[fallthrough]];
    case ExpandMacro::stage::main:
        t_stage = stage::buf_evaled;
        t_buf = link_params(*t_macro, t_param_list_b, t_param_list_e, env.farm());
        return f_eval_next(
            make_fnc<EvalLambda>(
                t_macro->body,
                begin(t_buf),
                end(t_buf),
                t_forse_noeval
            )
        );
    case ExpandMacro::stage::buf_evaled:
        t_buf = s_last_eval_val();
        if (auto m_opt = is_macro_call(t_buf, env)) {
            t_macro = &m_opt->get();
            t_param_list = cdr(t_buf);
            t_param_list_b = begin(t_param_list);
            t_param_list_e = end(t_param_list);
            t_stage = stage::main;
            return f_cycle();
        }
        return f_return(t_buf);
    default:
        throw logic_error("ExpandMacro::execute: unknown stage");
    }
}


EvalMacro::EvalMacro(
    
    const macro& macro,
    CarCdrIterator args_beg_it,
    CarCdrIterator args_end_it,
    bool forse_noeval
) :
    Func(func_id::evalmacro, true),
    t_buf( make_fnc<ExpandMacro>(macro, args_beg_it, args_end_it, forse_noeval))
{
}

void EvalMacro::execute(CoreEnvironment& env)
{
    switch (t_stage)
    {
    case EvalMacro::stage::start:
        t_stage = stage::ev;
        return f_eval_next(move(get<HolderPtr>(t_buf)));
    case EvalMacro::stage::ev:
        t_buf = s_last_eval_val();
        return f_return_next(make_fnc<EvalQuote>(get<Cell>(t_buf)));
    default:
        throw logic_error("EvalMacro::execute: unknown stage");
    }
}