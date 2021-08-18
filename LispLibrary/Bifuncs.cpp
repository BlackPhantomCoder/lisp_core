#include "Bifuncs.h"
#include "CoreEnv.h"
#include "Funcs.h"
#include "Symbol.h"
#include "DotPair.h"
#include "Number.h"
#include "LambdaFunc.h"
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


EvalQuoteRange::EvalQuoteRange(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it) :
    RangeNBiFunc(env, args_beg_it, args_end_it)
{
}

void EvalQuoteRange::t_init_after_args()
{
    t_it = t_args_beg();
    if (t_args_beg() == t_args_end()) {
        return t_return(t_env().t_farm.make_empty_list_cell());
    }

    return t_to_next();
}

void EvalQuoteRange::t_execute_func()
{
    if (t_it != t_args_end()) {
        t_result_v.push_back(t_last_eval_val());
        //return t_eval_next(make_fnc<EvalQuote>(t_env(), *t_it++));
        return t_to_next();
    }
    if(!t_last)t_result_v.push_back(t_last_eval_val());
    return t_return(t_env().t_farm.make_list_cell(begin(t_result_v), end(t_result_v)));
}

void EvalQuoteRange::t_to_next()
{
    auto* c = &*t_it++;
    while (t_it != t_args_end()) {
        if (!is_list(*c)) {
            t_result_v.push_back(t_env().eval_atom(*c));
        }
        else if (is_null_list(to_list(*c))) {
            t_result_v.push_back(*c);
        }
        else {
            break;
        }
        c = &*t_it++;
    }
    if (is_list(*c)) {
        if (is_null_list(to_list(*c))) {
            t_result_v.push_back(*c);
        }
        else {
            return t_eval_fnc(*c);
        }
    }
    else {
        t_result_v.push_back(t_env().eval_atom(*c));
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
   //return t_eval_next(make_fnc<EvalQuote>(t_env(), lst));
}

ProgN::ProgN(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it):
    RangeNBiFunc(env, args_beg_it, args_end_it)
{
}

void ProgN::t_init_after_args()
{
    t_it = t_args_beg();
    return t_cycle();
}

void ProgN::t_execute_func()
{
    if (t_it == t_args_end()) return t_return(t_env().t_farm.nil);
    //cout << "progn " << t_env.t_output_control.to_string(t_env.t_farm.make_list_cell(t_args_beg_it, t_args_end_it)) << endl;
    if(t_to_last) return t_to_last_f();

    auto& last = *t_it;
    if (t_imp) {
        if (is_null(t_last_eval_val())) {
            throw "nbifunc_progn error -> implicit cond at the end, but t_ferm.symbols->nil";
        }
        else {
            return t_return(car(t_last_eval_val()));
        }
    }
    if (is_implicit_cond(last, t_env().t_farm)) {
        t_imp = true;
        return  t_eval_next(make_fnc<ImplicitCond>(t_env(), last));
    }
    else {
        return t_return_next(make_fnc<EvalQuote>(t_env(), last));
    }
}

void ProgN::t_to_last_f()
{
    if (t_imp) {
        if (is_null(t_last_eval_val())) {
            t_imp = false;
            ++t_it;
            return t_cycle();
        }
        else {
            return t_return(car(t_last_eval_val()));
        }
    }
    if (next(t_it) != t_args_end()) {
        if (is_implicit_cond(*t_it, t_env().t_farm)) {
            t_imp = true;
            return  t_eval_next(make_fnc<ImplicitCond>(t_env(), *t_it));
        }
        else {
            t_args_buf = *t_it;
            ++t_it;
            return t_eval_next(
                make_fnc<EvalQuote>(
                    t_env(),
                    t_args_buf
                )
            );
        }
    }
    else {
        t_to_last = false;
        return t_cycle();
    }
}

Eval::Eval(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func):
    RangeBiFunc(env, args_beg_it, args_end_it, forse_noeval_func)
{
}

void Eval::t_execute_func()
{
    if (t_args_beg() == t_args_end()) return t_return(t_env().t_farm.nil);
    return t_return_next(
        make_fnc<EvalQuote>(
            t_env(),
            arg1
        )
    );
}

Cond::Cond(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it) :
    RangeNBiFunc(env, args_beg_it, args_end_it)
{
}

void Cond::t_init_after_args()
{
    t_it = t_args_beg();
    return t_cycle();
}

void Cond::t_execute_func()
{
    if (t_it == t_args_end()) return t_return(t_env().t_farm.nil);
    if (t_imp) {
        if (is_null(t_last_eval_val())) {
            t_imp = false;
            ++t_it;
            return t_cycle();
        }
        else {
            return t_return(car(t_last_eval_val()));
        }
    }
    auto& elem = *t_it;
    if (!is_implicit_cond(elem, t_env().t_farm)) {
        if (!is_list(elem) || is_null_list(to_list(elem)) || is_null(car(elem))) {
            ++t_it;
            return t_cycle();
        }
        auto& cdr_buf = cdr(elem);
        if (!is_list(cdr_buf) || is_null_list(to_list(cdr_buf)))
        {
            return t_return(car(elem));
        }
        return t_return_next(
            make_fnc<ProgN>(
                t_env(),
                begin(cdr_buf),
                end(cdr_buf)
            )
        );
    }
    t_imp = true;
    return t_eval_next(make_fnc<ImplicitCond>(t_env(), elem));
}

ImplicitCond::ImplicitCond(CoreEnvironment& env, Cell& atom):
    Func(env),
    t_atom(atom)
{
}

void ImplicitCond::t_init_after_args()
{
    return t_eval_next(
        make_fnc<EvalQuote>(
            t_env(),
            car(t_atom)
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
        return t_return(t_env().t_farm.make_list_cell(t_last_eval_val()));
    }

    if (empty(t_predicate_val_buf)) {
        t_predicate_val_buf = t_last_eval_val();
    }
    //cout << "cond " << t_env.t_output_control.to_string(t_atom) << endl;
    //cout << "cond " << t_env.t_output_control.to_string(*t_predicate_val_buf) << endl;

    if (!is_null(t_predicate_val_buf)) {
        //cout << "cond " << t_env.t_output_control.to_string(cdr(t_atom)) << endl;
        auto& cdr_buf = cdr(t_atom);
        if (!is_list(cdr_buf) || is_null(cdr_buf))
        {
            return t_return(t_env().t_farm.make_list_cell(t_predicate_val_buf));
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

    return t_return(t_env().t_farm.nil);
}

EvalQuote::EvalQuote(CoreEnvironment& env, Cell& c):
    Func(env),
    t_arg(c)
{
}

bool EvalQuote::t_eval_args()
{
    return true;
}

void EvalQuote::t_internal_execute()
{
    if (is_list(t_arg)) {
        if (is_null_list(to_list(t_arg))) return t_return(t_env().t_farm.nil);
        if (auto& cdr_buf = cdr(t_arg); is_list(cdr_buf)) {
            return t_eval_func(car(t_arg), begin(cdr_buf), end(cdr_buf));
        }
        else {
            //if args is dotpair
            return t_eval_func(car(t_arg), CarCdrIterator(), CarCdrIterator());
        }
    }
    else {
        return t_return(
            t_env().eval_atom(t_arg)
        );
    }
}

void EvalQuote::t_eval_func(Cell& fnc, CarCdrIterator args_beg_it, CarCdrIterator args_end_it)
{
    return t_return_next(make_fnc<EvalFunc>(t_env(), fnc, args_beg_it, args_end_it));
}


EvalFunc::EvalFunc(CoreEnvironment& env, Cell& fnc, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func):
    Func(env),
    t_args_beg(args_beg_it),
    t_args_end(args_end_it),
    t_fnc(fnc),
    t_forse_noeval_func(forse_noeval_func)
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

    if (is_lambda_form(t_fnc, t_env().t_farm)) {
        auto lambda = t_env().get_lambda_form(t_fnc);
        return t_return_next(
            make_fnc<ALambdaFunc>(
                t_env(),
                (lambda).params,
                (lambda).body,
                t_args_beg,
                t_args_end,
                ((lambda).type == lambda_types::lambda) ? eval_types::eval : eval_types::no_eval,
                ((lambda).arg_type == lambda_args_types::spread) ? spread_types::spread : spread_types::nospread,
                t_forse_noeval_func
                )
        );
    }

    if (!is_symbol(t_fnc)) throw string("eval_fnc: unknown function ") + t_env().t_output_control.to_string(t_fnc);
    if (auto fnc_r = t_env().t_funcs.find(to_symbol(t_fnc))) {
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
                       make_fnc<ALambdaFunc>(
                           t_env(),
                           arg.params,
                           arg.body,
                           t_args_beg,
                           t_args_end,
                           (arg.type == lambda_types::lambda) ? eval_types::eval : eval_types::no_eval,
                           (arg.arg_type == lambda_args_types::spread) ? spread_types::spread : spread_types::nospread,
                           t_forse_noeval_func
                           )
                    );
                },
                [this](const CoreData::special_bifunc_make& arg) { 
                    return t_return_next(arg(t_env(), t_args_beg, t_args_end, t_forse_noeval_func));
                },
                [this](const CoreData::special_nbifunc_make& arg) {
                     return t_return_next(arg(t_env(), t_args_beg, t_args_end));
                }
                    
        }, fnc_r->get());
    }
    else {
        throw "eval_fnc error " + t_env().t_output_control.to_string(
            t_env().t_farm.make_list_cell({ t_fnc, t_env().t_farm.make_list_cell(t_args_beg , t_args_end) })
        );
    }
}

Prog1::Prog1(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it) :
    RangeNBiFunc(env, args_beg_it, args_end_it)
{
}

void Prog1::t_execute_func()
{
    if (t_args_beg() == t_args_end()) return t_return(t_env().t_farm.nil);
    //cout << "prog1 " << t_env.t_output_control.to_string(t_env.t_farm.make_list_cell(t_args_beg_it, t_args_end_it)) << endl;
    if (t_progn) {
        return t_return(t_result_buf);
    }

    if (t_ev) {
        t_ev = false;
        t_result_buf = t_last_eval_val();
        t_progn = true;
        return t_eval_next(make_fnc<ProgN>(t_env(), next(t_args_beg()), t_args_end()));
    }

    if (t_imp) {
        t_imp = false;
        if (is_null(t_last_eval_val())) {
            throw "nbifunc_progn error -> implicit cond at the end, but t_ferm.symbols->nil";
        }
        else {
            t_result_buf = car(t_last_eval_val());
        }
        t_progn = true;
        return t_eval_next(make_fnc<ProgN>(t_env(), next(t_args_beg()), t_args_end()));
    }

    if (is_implicit_cond(*t_args_beg(), t_env().t_farm)) {
        t_imp = true;
        return t_eval_next(make_fnc<ImplicitCond>(t_env(), *t_args_beg()));
    }
    else {
        t_ev = true;
        return t_eval_next(make_fnc<EvalQuote>(t_env(), *t_args_beg()));
    }
}

Append::Append(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func):
    RangeBiFunc(env, args_beg_it, args_end_it, forse_noeval_func)
{
}

void Append::t_init_after_args()
{
    if (t_args_beg() == t_args_end()) t_return(t_env().t_farm.nil);
    t_it = t_args_beg();
    return t_cycle();
}

void Append::t_execute_func()
{
    while (t_it != t_args_end() && (is_atom(*t_it) || is_null_list(to_list(*t_it)))) {
        ++t_it;
    }
    if(t_it == t_args_end()) return t_return(t_env().t_farm.nil);
    if(next(t_it) == t_args_end()) return t_return(*t_it);
    Cell s = *t_it;
    ++t_it;
    for(; t_it != t_args_end(); ++t_it){
        s = append(s, *t_it, t_env().t_farm);
    }
    return t_return(s);

}

Loop::Loop(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it) :
    RangeNBiFunc(env, args_beg_it, args_end_it)
{
}

void Loop::t_init_after_args()
{
    t_it = t_args_beg();
    return t_cycle();
}

void Loop::t_execute_func()
{
    if (t_imp) {
        t_imp = false;
        if (!is_null(t_last_eval_val())) {
            auto& res = car(t_last_eval_val());
            if(!is_null(res)) return t_return(res);
        }
    }

    if(t_it == t_args_end()) t_it = t_args_beg();
    if (is_implicit_cond(*t_it, t_env().t_farm)) {
        t_imp = true;
        return t_eval_next(make_fnc<ImplicitCond>(t_env(), *t_it++));
    }
    else {
        return t_eval_next(make_fnc<EvalQuote>(t_env(), *t_it++));
    }
}

Apply::Apply(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval) :
    RangeBiFunc(env, args_beg_it, args_end_it, forse_noeval)
{
}

void Apply::t_init_after_args()
{
    if (t_args_beg() == t_args_end()) throw "bifunc_apply error (unknown function)";
    return t_cycle();
}

void Apply::t_execute_func()
{
    if (ArgsCounter{t_args_beg(), t_args_end()} >= 2) {
        if(is_list(arg2)) {
            return t_return_next(make_fnc<EvalFunc>(t_env(), arg1, begin(arg2), end(arg2), true));
        }
        t_eval_buf = t_env().t_farm.make_list_cell({ arg1 });
        return t_return_next(make_fnc<EvalQuote>(t_env(), t_eval_buf));
    }
    else {
        t_eval_buf = t_env().t_farm.make_list_cell({ arg1 });
        return t_return_next(make_fnc<EvalQuote>(t_env(), t_eval_buf));
    }
}

SetQ::SetQ(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it) :
    RangeNBiFunc(env, args_beg_it, args_end_it)
{
}

void SetQ::t_init_after_args()
{
    if (t_args_beg() == t_args_end()) return t_return(t_env().t_farm.nil);
    if (next(t_args_beg()) == t_args_end()) {
        t_env().set_value(*t_args_beg(), t_env().t_farm.nil);
        return t_return(t_env().t_farm.nil);
    }
    t_it = next(t_args_beg());
    return t_cycle();
}

void SetQ::t_execute_func()
{
    if(!t_ev && t_it != t_args_end()){
        t_ev = true;
        return t_eval_next(make_fnc<EvalQuote>(t_env(), *t_it));
    }

    auto t_val = t_last_eval_val();

    t_env().set_value(*t_args_beg(), t_val);
    if (t_it != t_args_end() && next(t_it, 1) != t_args_end()) {
        return t_return_next(make_fnc<SetQ>(t_env(), next(t_it, 1), t_args_end()));
    }
    else {
        return t_return(t_val);
    }
}
