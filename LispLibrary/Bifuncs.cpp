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



void Prog1::execute(CoreEnvironment& env)
{
    switch (t_stage)
    {
    case stage::start:
        if (args_beg() == args_end())
            return f_return(env.farm().nil());
        else {
            if (is_implicit_cond(*args_beg(), env.farm())) {
                t_stage = stage::imp;
                return f_eval_next(make_fnc<ImplicitCond>(*args_beg()));
            }
            else {
                t_stage = stage::ev;
                return f_eval_next(make_fnc<EvalQuote>(*args_beg()));
            }
        }
    case stage::imp:
        if (is_null(s_last_eval_val())) {
            throw "nbifunc_progn error -> implicit cond at the end, but t_ferm.symbols->nil";
        }
        else {
            t_result_buf = car(s_last_eval_val());
        }
        t_stage = stage::progn;
        return f_eval_next(make_fnc<ProgN>(next(args_beg()), args_end()));
    case stage::ev:
        t_result_buf = s_last_eval_val();
        t_stage = stage::progn;
        return f_eval_next(make_fnc<ProgN>(next(args_beg()), args_end()));
    case stage::progn:
        return f_return(t_result_buf);
    default:
        throw logic_error(" Prog1::execute: unknown stage");
    }
}


void ProgN::execute(CoreEnvironment& env)
{
    switch (t_stage)
    {
    case ProgN::stage::start:
        t_it = args_beg();
        if (t_it == args_end()) 
            return f_return(env.farm().nil());
        t_stage = stage::eval;
        [[fallthrough]];
    case ProgN::stage::eval:
        if (next(t_it) != args_end()) {
            if (is_implicit_cond(*t_it, env.farm())) {
                t_stage = stage::eval_imp;
                return  f_eval_next(make_fnc<ImplicitCond>(*t_it));
            }
            else {
                t_args_buf = *t_it;
                ++t_it;
                return f_eval_next(make_fnc<EvalQuote>(t_args_buf));
            }
        }
        else {
            t_stage = stage::last;
            return f_cycle();
        }
    case ProgN::stage::eval_imp:
        if (is_null(s_last_eval_val())) {
            t_stage = stage::eval;
            ++t_it;
            return f_cycle();
        }
        else {
            return f_return(car(s_last_eval_val()));
        }
    case ProgN::stage::last_imp:
        if (is_null(s_last_eval_val())) {
            throw "nbifunc_progn error -> implicit cond at the end, but t_ferm.symbols->nil";
        }
        else {
            return f_return(car(s_last_eval_val()));
        }
    case ProgN::stage::last:
        {
            auto& last = *t_it;
            if (is_implicit_cond(last, env.farm())) {
                t_stage = stage::last_imp;
                return  f_eval_next(make_fnc<ImplicitCond>(last));
            }
            else {
                return f_return_next(make_fnc<EvalQuote>(last));
            }
        }
    default:
        throw logic_error(" ProgN::execute: unknown stage");
    }
}

Eval::Eval(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func):
    RangeBiFunc(func_id::eval, args_beg_it, args_end_it, forse_noeval_func)
{
}

void Eval::execute(CoreEnvironment& env)
{
    if (args_beg() == args_end()) return f_return(env.farm().nil());
    return f_return_next(make_fnc<EvalQuote>(arg1));
}

void Cond::execute(CoreEnvironment& env)
{
    switch (t_stage)
    {
    case Cond::stage::start:
        t_it = args_beg();
        t_stage = stage::main;
        [[fallthrough]];
    case Cond::stage::main:
        {
            if (t_it == args_end()) return f_return(env.farm().nil());
            auto& elem = *t_it;
            if (!is_implicit_cond(elem, env.farm())) {
                if (!is_list(elem) || is_null_list(to_list(elem))) {
                    ++t_it;
                    return f_cycle();
                }
                t_stage = stage::ev;
                return f_eval_next(make_fnc<EvalQuote>(car(elem)));
            }
            t_stage = stage::imp;
            return f_eval_next(make_fnc<ImplicitCond>(elem));
        }
    case Cond::stage::imp:
    {
        if (is_null(s_last_eval_val())) {
            t_stage = stage::main;
            ++t_it;
            return f_cycle();
        }
        else {
            return f_return(car(s_last_eval_val()));
        }
    }
    case Cond::stage::ev:
        {
            t_stage = stage::main;
            auto& elem = *t_it;
            t_buf = s_last_eval_val();
            if (!is_null(t_buf)) {
                auto& cdr_buf = cdr(elem);
                if (!is_list(cdr_buf) || is_null_list(to_list(cdr_buf)))
                {
                    return f_return(t_buf);
                }
                return f_return_next(
                    make_fnc<ProgN>(
                        begin(cdr_buf),
                        end(cdr_buf)
                    )
                );
            }
            else {
                ++t_it;
                return f_cycle();
            }
        }
    default:
        throw logic_error("Cond::execute: unknown stage");
    }
}

Append::Append(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func):
    RangeBiFunc(func_id::append, args_beg_it, args_end_it, forse_noeval_func)
{
}

void Append::after_args_eval(CoreEnvironment& env)
{
    if (args_beg() == args_end()) return f_return(env.farm().nil());
    t_it = args_beg();
}

void Append::execute(CoreEnvironment& env)
{
    while (t_it != args_end() && (is_atom(*t_it) || is_null_list(to_list(*t_it)))) {
        ++t_it;
    }
    if(t_it == args_end()) return f_return(env.farm().nil());
    if(next(t_it) == args_end()) return f_return(*t_it);
    Cell s = *t_it;
    ++t_it;
    for(; t_it != args_end(); ++t_it){
        s = append(s, *t_it, env.farm());
    }
    return f_return(s);

}

void Loop::execute(CoreEnvironment& env)
{
    switch (t_stage)
    {
    case Loop::stage::start:
        t_it = args_beg();
        [[fallthrough]];
    case Loop::stage::main:
        if (t_it == args_end()) 
            t_it = args_beg();
        if (is_implicit_cond(*t_it, env.farm())) {
            t_stage = stage::imp;
            return f_eval_next(make_fnc<ImplicitCond>(*t_it++));
        }
        else {
            return f_eval_next(make_fnc<EvalQuote>(*t_it++));
        }
    case Loop::stage::imp:
        if (!is_null(s_last_eval_val())) {
            auto& res = car(s_last_eval_val());
            if (!is_null(res)) 
                return f_return(res);
        }
        t_stage = stage::main;
        return f_cycle();
    default:
        throw logic_error("Loop::execute: unknown stage");
    }
}

Apply::Apply(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval) :
    RangeBiFunc(func_id::apply, args_beg_it, args_end_it, forse_noeval)
{
}

void Apply::after_args_eval(CoreEnvironment& env)
{
    if (args_beg() == args_end()) throw "bifunc_apply error (unknown function)";
}

void Apply::execute(CoreEnvironment& env)
{
    if (ArgsCounter{args_beg(), args_end()} >= 2) {
        if(is_list(arg2)) {
            auto& buf = arg2;
            return f_return_next(make_fnc<EvalFunc>(arg1, begin(buf), end(buf), true));
        }
        t_eval_buf = env.farm().make_list_cell({ arg1 });
        return f_return_next(make_fnc<EvalQuote>(t_eval_buf));
    }
    else {
        t_eval_buf = env.farm().make_list_cell({ arg1 });
        return f_return_next(make_fnc<EvalQuote>(t_eval_buf));
    }
}

void SetQ::execute(CoreEnvironment& env)
{
    switch (t_stage)
    {
    case SetQ::stage::start:
        t_it = args_beg();
        if (t_it == args_end())
            return f_return(env.farm().nil());
        [[fallthrough]];
    case SetQ::stage::repeat_start:
        if (next(t_it) == args_end()) {
            env.core_funcs().set_value(*t_it, env.farm().nil());
            return f_return(env.farm().nil());
        }
        [[fallthrough]];
    case SetQ::stage::main:
        t_stage = stage::ev;
        return f_eval_next(make_fnc<EvalQuote>(*next(t_it)));
    case SetQ::stage::ev:
        {
            auto t_val = s_last_eval_val();
            env.core_funcs().set_value(*t_it, t_val);
            t_it = next(t_it, 2);
            if (t_it != args_end()) {
                t_stage = stage::repeat_start;
                return f_cycle();
            }
            else {
                return f_return(t_val);
            }
        }
    default:
        throw logic_error("SetQ::execute: unknown stage");
    }
}


MacroExpand1::MacroExpand1(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval) :
    RangeBiFunc(func_id::macroexpand1, args_beg_it, args_end_it, forse_noeval),
    t_forse_noeval(forse_noeval)
{
}

void MacroExpand1::after_args_eval(CoreEnvironment& env)
{
    if (args_beg() == args_end()) f_return(env.farm().nil());
}

void MacroExpand1::execute(CoreEnvironment& env)
{   
    if (auto m_opt = is_macro_call(arg1, env)) {
        auto& cd = cdr(arg1);
        return f_return_next(
            make_fnc<ExpandMacro>(
                m_opt->get(),
                begin(cd),
                end(cd),
                t_forse_noeval,
                true
            )
        );
    }
    return f_return(arg1);
}

MacroExpand::MacroExpand(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval) :
    RangeBiFunc(func_id::macroexpand, args_beg_it, args_end_it, forse_noeval),
    t_forse_noeval(forse_noeval)
{
}

void MacroExpand::after_args_eval(CoreEnvironment& env)
{
    if (args_beg() == args_end()) f_return(env.farm().nil());
}

void MacroExpand::execute(CoreEnvironment& env)
{
    if (auto m_opt = is_macro_call(arg1, env)) {
        auto& cd = cdr(arg1);
        return f_return_next(
            make_fnc<ExpandMacro>(
                m_opt->get(),
                begin(cd),
                end(cd),
                t_forse_noeval,
                false
            )
        );
    }
    return f_return(arg1);
}

MapCar::MapCar(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval) :
    RangeBiFunc(func_id::mapcar, args_beg_it, args_end_it, forse_noeval)
{
}

void MapCar::after_args_eval(CoreEnvironment& env)
{
    if (args_beg() == args_end()) return  f_return(env.farm().nil());
    if (next(args_beg()) == args_end())return  f_return(env.farm().nil());
    t_func = *args_beg();

    bool nil = false;
    for (auto it = next(args_beg()); it != args_end(); ++it) {
        if (is_null(*it) || !is_list(*it)) {
            nil = true;
            break;
        }
        else {
            t_lists.push_back(*it);
        }
    }
    if(nil) return f_return(env.farm().nil());
}

void MapCar::execute(CoreEnvironment& env)
{
    if (t_ev) {
        t_ev = false;
        t_result.push_back(s_last_eval_val());
    }

    if (t_finish) {
        return f_return(env.farm().make_list_cell(begin(t_result), end(t_result)));
    }

    std::vector<Cell> buf;
    for (auto it = begin(t_lists); it != end(t_lists); ++it) {
        buf.push_back(car(*it));
        auto& cd = cdr(*it);
        if (is_null(cd) || !is_list(cd)) {
            t_finish = true;
        }
        else {
            *it = cd;
        }
    }
    t_buf = env.farm().make_list_cell(begin(buf), end(buf));
    t_ev = true;
    return f_eval_next(make_fnc<EvalFunc>(t_func, begin(t_buf), end(t_buf)));
}

Read::Read(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func):
    RangeBiFunc(func_id::read, args_beg_it, args_end_it, forse_noeval_func)
{
    t_parser = yypstate_new();
}

void Read::before_args_eval(CoreEnvironment& env)
{
    t_hp.set_env(env);
}

void Read::execute(CoreEnvironment& env)
{
    try
    {
        unsigned long parse_val;
        bool macro_final = false;
        auto code = 0;
        auto val = optional<Cell>();

        //если запрошен макрос
        if (t_macro) {
            t_macro = false;
            val = s_last_eval_val();
            //cout << env.output_control().to_string(*val) << endl;
            
           /*if (env.read_supp().read_final_supp.size() != t_last_finals_count) {
                macro_final = true;
                code = tokens::FinalMacro;
            }
            else {
                code = tokens::Macro;
            }*/
            code = tokens::Macro;
        }
        else {

            // получаем токен
            tie(code, val) = env.scanner().lex();
            // запрашиваем макрос
            if (code == tokens::Macro) {
                t_macro = true;
                t_buf = *val;
                //cout << env.output_control().to_string(*val) << endl;
                return this->f_eval_next(make_fnc<EvalQuote>(t_buf));
            }
        }
        // сохран€ем Cell 
        if (val) {
            parse_val = t_hp.push_cell(*val);
        }
        // пушим
        auto status = yypush_parse(t_parser, code, &parse_val, &t_hp);
        // продолжаем пушить
        if (status == tokens::YYPUSH_MORE) return f_cycle();
        // если конец файла
        if (code == 0 && status != 0) {
            throw errors::eos{};
        }
        // синтаксическа€ ошибка
        if(status != 0)  
            throw errors::syntax_error{};
        yypstate_delete(t_parser);
        t_parser = nullptr;
        // доп проверка на специальные макрочары
        if (t_hp.is_saved_macro()) {
            return f_return_next(make_fnc<EvalQuote>(t_hp.get_saved()));
        }
        // возвращаем
        return f_return(t_hp.get_saved());
    }
    catch (...) {
        if (t_parser) {
            yypstate_delete(t_parser);
            t_parser = nullptr;
        }
        throw;
    }
}

PeekChar::PeekChar(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func):
    RangeBiFunc(func_id::peekchar, args_beg_it, args_end_it, forse_noeval_func)
{
}

void PeekChar::after_args_eval(CoreEnvironment& env)
{
    if (!env.input().alive()) throw "peek_char: empty stream";
    if (is_symbol(arg1) && !is_null(arg1)) {
        t_skip_comments = to_symbol(arg1) == CoreData::T_str;
        if (!t_skip_comments) {
            t_until = true;
            auto raw = env.output_control().to_string_raw(arg1);
            if (empty(raw)) throw "empty symbol";
            t_until_char = raw[0];
        }
    }
}

//!!!!!
//ƒл€ удобства программировани€ символ, выдаваемый PEEK-CHAR, кроме того, присваиваетс€ переменной RATOM
void PeekChar::execute(CoreEnvironment& env)
{
    //until_char
    if (t_until_char) {
        if (t_until_char_cycle) {
            if (t_i == -1) {
                //stream set eos
                env.input().read_char();
                throw errors::eos{};
            }
            auto ch = char(t_i);
            while (ch != t_until_char) {
                t_i = env.input().read_char();
            }
            env.input().unread_char();
            return f_return(env.farm().make_symbol_cell(string() + t_until_char));
        }
        t_until_char_cycle = true;
        t_i = env.input().read_char();
        return f_cycle();
    }
    else {
        //skip_comments | peek

        if (t_skip_comments_cycle) {
            if (t_i == -1) {
                //stream set eos
                env.input().read_char();
                throw errors::eos{};
            }
            auto ch = char(t_i);
            if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
                env.input().read_char();
                t_i = env.input().peek_char();
                return f_cycle();
            }
            return f_return(env.farm().make_symbol_cell(string() + ch));
        }

        if (t_skip_comments) {
            t_skip_comments_cycle = true;
            t_i = env.input().peek_char();
            return f_cycle();
        }

        auto i = env.input().peek_char();
        if (i == -1) {
            //stream set eos
            env.input().read_char();
            throw errors::eos{};
        }

        return f_return(env.farm().make_symbol_cell(string() + char(i)));
    }
}
