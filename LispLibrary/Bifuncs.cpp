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
    if (t_it == t_args_end()) return t_return(t_env().farm().nil());
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
    if (is_implicit_cond(last, t_env().farm())) {
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
        if (is_implicit_cond(*t_it, t_env().farm())) {
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
    if (t_args_beg() == t_args_end()) return t_return(t_env().farm().nil());
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
    if (t_it == t_args_end()) return t_return(t_env().farm().nil());
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
    if (t_ev) {
        t_ev = false;
        auto& elem = *t_it;
        t_buf = t_last_eval_val();
        if (!is_null(t_buf)) {
            auto& cdr_buf = cdr(elem);
            if (!is_list(cdr_buf) || is_null_list(to_list(cdr_buf)))
            {
                return t_return(t_buf);
            }
            return t_return_next(
                make_fnc<ProgN>(
                    t_env(),
                    begin(cdr_buf),
                    end(cdr_buf)
                    )
            );
        }
        else {
            ++t_it;
            return t_cycle();
        }
    }

    auto& elem = *t_it;
    if (!is_implicit_cond(elem, t_env().farm())) {
        if (!is_list(elem) || is_null_list(to_list(elem))) {
            ++t_it;
            return t_cycle();
        }
        t_ev = true;
        return t_eval_next(make_fnc<EvalQuote>(t_env(), car(elem)));
    }
    t_imp = true;
    return t_eval_next(make_fnc<ImplicitCond>(t_env(), elem));
}

Prog1::Prog1(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it) :
    RangeNBiFunc(env, args_beg_it, args_end_it)
{
}

void Prog1::t_execute_func()
{
    if (t_args_beg() == t_args_end()) return t_return(t_env().farm().nil());
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

    if (is_implicit_cond(*t_args_beg(), t_env().farm())) {
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
    if (t_args_beg() == t_args_end()) t_return(t_env().farm().nil());
    t_it = t_args_beg();
    return t_cycle();
}

void Append::t_execute_func()
{
    while (t_it != t_args_end() && (is_atom(*t_it) || is_null_list(to_list(*t_it)))) {
        ++t_it;
    }
    if(t_it == t_args_end()) return t_return(t_env().farm().nil());
    if(next(t_it) == t_args_end()) return t_return(*t_it);
    Cell s = *t_it;
    ++t_it;
    for(; t_it != t_args_end(); ++t_it){
        s = append(s, *t_it, t_env().farm());
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
    if (is_implicit_cond(*t_it, t_env().farm())) {
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
            auto& buf = arg2;
            return t_return_next(make_fnc<EvalFunc>(t_env(), arg1, begin(buf), end(buf), true));
        }
        t_eval_buf = t_env().farm().make_list_cell({ arg1 });
        return t_return_next(make_fnc<EvalQuote>(t_env(), t_eval_buf));
    }
    else {
        t_eval_buf = t_env().farm().make_list_cell({ arg1 });
        return t_return_next(make_fnc<EvalQuote>(t_env(), t_eval_buf));
    }
}

SetQ::SetQ(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it) :
    RangeNBiFunc(env, args_beg_it, args_end_it)
{
}

void SetQ::t_init_after_args()
{
    if (t_args_beg() == t_args_end()) return t_return(t_env().farm().nil());
    if (next(t_args_beg()) == t_args_end()) {
        t_env().support_funcs().set_value(*t_args_beg(), t_env().farm().nil());
        return t_return(t_env().farm().nil());
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

    t_env().support_funcs().set_value(*t_args_beg(), t_val);
    if (t_it != t_args_end() && next(t_it, 1) != t_args_end()) {
        return t_return_next(make_fnc<SetQ>(t_env(), next(t_it, 1), t_args_end()));
    }
    else {
        return t_return(t_val);
    }
}


MacroExpand1::MacroExpand1(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval) :
    RangeBiFunc(env, args_beg_it, args_end_it, forse_noeval),
    t_forse_noeval(forse_noeval)
{
}

void MacroExpand1::t_init_after_args()
{
    if (t_args_beg() == t_args_end()) t_return(t_env().farm().nil());
}

void MacroExpand1::t_execute_func()
{   
    if (auto m_opt = is_macro_call(arg1, t_env())) {
        auto& cd = cdr(arg1);
        return t_return_next(
            make_fnc<ExpandMacro>(
                t_env(),
                m_opt->get(),
                begin(cd),
                end(cd),
                t_forse_noeval,
                true
            )
        );
    }
    return t_return(arg1);
}

MacroExpand::MacroExpand(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval) :
    RangeBiFunc(env, args_beg_it, args_end_it, forse_noeval),
    t_forse_noeval(forse_noeval)
{
}

void MacroExpand::t_init_after_args()
{
    if (t_args_beg() == t_args_end()) t_return(t_env().farm().nil());
}

void MacroExpand::t_execute_func()
{
    if (auto m_opt = is_macro_call(arg1, t_env())) {
        auto& cd = cdr(arg1);
        return t_return_next(
            make_fnc<ExpandMacro>(
                t_env(),
                m_opt->get(),
                begin(cd),
                end(cd),
                t_forse_noeval,
                false
            )
        );
    }
    return t_return(arg1);
}

MapCar::MapCar(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval) :
    RangeBiFunc(env, args_beg_it, args_end_it, forse_noeval)
{
}

void MapCar::t_init_after_args()
{
    if (t_args_beg() == t_args_end()) return  t_return(t_env().farm().nil());
    if (next(t_args_beg()) == t_args_end())return  t_return(t_env().farm().nil());
    t_func = *t_args_beg();

    bool nil = false;
    for (auto it = next(t_args_beg()); it != t_args_end(); ++it) {
        if (is_null(*it) || !is_list(*it)) {
            nil = true;
            break;
        }
        else {
            t_lists.push_back(*it);
        }
    }
    if(nil) return t_return(t_env().farm().nil());
}

void MapCar::t_execute_func()
{
    if (t_ev) {
        t_ev = false;
        t_result.push_back(t_last_eval_val());
    }

    if (t_finish) {
        return t_return(t_env().farm().make_list_cell(begin(t_result), end(t_result)));
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
    t_buf = t_env().farm().make_list_cell(begin(buf), end(buf));
    t_ev = true;
    return t_eval_next(make_fnc<EvalFunc>(t_env(), t_func, begin(t_buf), end(t_buf)));
}

Read::Read(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func):
    RangeBiFunc(env, args_beg_it, args_end_it, forse_noeval_func),
    t_hp(env)
{
    t_parser = yypstate_new();
}

void Read::t_execute_func()
{
    try
    {
        unsigned long parse_val;
        bool macro_final = false;
        auto code = 0;
        auto val = optional<Cell>();

        //���� �������� ������
        if (t_macro) {
            t_macro = false;
            val = t_last_eval_val();
            //cout << t_env().output_control().to_string(*val) << endl;
            
           /*if (t_env().read_supp().read_final_supp.size() != t_last_finals_count) {
                macro_final = true;
                code = tokens::FinalMacro;
            }
            else {
                code = tokens::Macro;
            }*/
            code = tokens::Macro;
        }
        else {

            // �������� �����
            tie(code, val) = t_env().scanner().lex();
            // ����������� ������
            if (code == tokens::Macro) {
                t_macro = true;
                t_buf = *val;
                //cout << t_env().output_control().to_string(*val) << endl;
                return this->t_eval_next(make_fnc<EvalQuote>(t_env(), t_buf));
            }
        }
        // ��������� Cell 
        if (val) {
            parse_val = t_hp.push_cell(*val);
        }
        // �����
        auto status = yypush_parse(t_parser, code, &parse_val, &t_hp);
        // ���������� ������
        if (status == tokens::YYPUSH_MORE) return t_cycle();
        // ���� ����� �����
        if (code == 0 && status != 0) {
            throw errors::eos{};
        }
        // �������������� ������
        if(status != 0)  
            throw errors::syntax_error{};
        yypstate_delete(t_parser);
        t_parser = nullptr;
        // ��� �������� �� ����������� ���������
        if (t_hp.is_saved_macro()) {
            return t_return_next(make_fnc<EvalQuote>(t_env(), t_hp.get_saved()));
        }
        // ����������
        return t_return(t_hp.get_saved());
    }
    catch (...) {
        if (t_parser) {
            yypstate_delete(t_parser);
            t_parser = nullptr;
        }
        throw;
    }
}

PeekChar::PeekChar(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func):
    RangeBiFunc(env, args_beg_it, args_end_it, forse_noeval_func)
{
}

void PeekChar::t_init_after_args()
{
    if (!t_env().input().alive()) throw "peek_char: empty stream";
    if (is_symbol(arg1) && !is_null(arg1)) {
        t_skip_comments = to_symbol(arg1) == CoreData::T_str;
        if (!t_skip_comments) {
            t_until = true;
            auto raw = t_env().output_control().to_string_raw(arg1);
            if (empty(raw)) throw "empty symbol";
            t_until_char = raw[0];
        }
    }
}


//!!!!!
//��� �������� ���������������� ������, ���������� PEEK-CHAR, ����� ����, ������������� ���������� RATOM
void PeekChar::t_execute_func()
{
    //until_char
    if (t_until_char) {
        if (t_until_char_cycle) {
            if (t_i == -1) {
                //stream set eos
                t_env().input().read_char();
                throw errors::eos{};
            }
            auto ch = char(t_i);
            while (ch != t_until_char) {
                t_i = t_env().input().read_char();
            }
            t_env().input().unread_char();
            return t_return(t_env().farm().make_symbol_cell(string() + t_until_char));
        }
        t_until_char_cycle = true;
        t_i = t_env().input().read_char();
        return t_cycle();
    }
    else {
        //skip_comments | peek

        if (t_skip_comments_cycle) {
            if (t_i == -1) {
                //stream set eos
                t_env().input().read_char();
                throw errors::eos{};
            }
            auto ch = char(t_i);
            if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
                t_env().input().read_char();
                t_i = t_env().input().peek_char();
                return t_cycle();
            }
            return t_return(t_env().farm().make_symbol_cell(string() + ch));
        }

        if (t_skip_comments) {
            t_skip_comments_cycle = true;
            t_i = t_env().input().peek_char();
            return t_cycle();
        }

        auto i = t_env().input().peek_char();
        if (i == -1) {
            //stream set eos
            t_env().input().read_char();
            throw errors::eos{};
        }

        return t_return(t_env().farm().make_symbol_cell(string() + char(i)));
    }
}
