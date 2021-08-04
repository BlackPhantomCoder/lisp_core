#include "CoreEnv.h"

#include "BigInt.h"
#include "Funcs.h"
#include "BIFuncEvaler.h"
#include "Lexer.h"

#include <list>
#include <iostream>
#include <unordered_map>
#include <variant>
#include <cmath>
#include <algorithm>
#include <sstream>

using namespace std;
using namespace CoreData;

#define arg1 *begin(t_args)
#define arg2 *(begin(t_args) + 1)
#define arg3 *(begin(t_args) + 2)


__forceinline lambda make_spread_lambda_form(
    DPair::const_iterator beg_params,
    DPair::const_iterator end_params,
    DPair::const_iterator beg_body,
    DPair::const_iterator end_body
)
{
    auto size = std::distance(beg_params, end_params);
    std::vector<Symbol> params;
    params.reserve(size);
    for (DPair::const_iterator it = beg_params; it != end_params; ++it) {
        if (!is_symbol(*it)) throw "get_lambda_form error";
        params.emplace_back(to_symbol(*it));
    }

    return  make_lambda(lambda_types::lambda, lambda_args_types::spread, move(params), { beg_body , end_body });
}

__forceinline lambda make_nospread_lambda_form(
    lambda_types lambda_type,
    Symbol param,
    DPair::const_iterator beg_body,
    DPair::const_iterator end_body
)
{
    return  make_lambda(lambda_type, lambda_args_types::nospread, { move(param) }, { beg_body , end_body });
}

__forceinline lambda get_lambda_form(
    DPair::const_iterator beg_it,
    DPair::const_iterator end_it
)
{
    if (std::distance(beg_it, end_it) < 2) throw "get_lambda_form error";
    if (is_list(*(beg_it + 1))) {
        return make_spread_lambda_form(
            begin(to_list(*(beg_it + 1))), end(to_list(*(beg_it + 1))),
            beg_it + 2, end_it
        );
    }
    else if(is_symbol(*(beg_it + 1))){
        return make_nospread_lambda_form(
            (to_symbol(*beg_it) == CoreData::lambda_str) ? lambda_types::lambda : lambda_types::nlambda,
            to_symbol(*(beg_it+1)),
            beg_it + 2, end_it
        );
    }
    throw "get_lambda_form error";
}

__forceinline void set_value(CoreEnvironment& env, const Cell& name, const Cell& val)
{
    if (!is_symbol(name)) {
        if (!is_null(name, env.t_symbols)) {
            throw "bifunc_set error";
        }
    }
    else {
        const auto& symb = to_symbol(name);
        if (auto var = env.t_envs.get(symb)) {
            *var = val;
        }
        else {
            env.t_envs.add_global_var(symb, val);
        }
        if (symb == env.t_symbols.read_up_case_symbol) {
            if (is_null(val, env.t_symbols)) {
                env.t_syntaxer.set_upcase_mode(false);
            }
        }
    }
}

__forceinline Cell eval_fnc(
    CoreEnvironment& env,
    const Cell& fnc,
    DPair::const_iterator args_beg_it,
    DPair::const_iterator args_end_it,
    bool forse_nospread_args
)
{                                                                               
    if (is_alambda_form(fnc, env.t_symbols)) {
        auto l = get_lambda_form(begin(to_list(fnc)), end(to_list(fnc)));       
        env.t_l_evaler.push({ l, args_beg_it,args_end_it,forse_nospread_args });
        return env.t_l_evaler.pop_eval();
    }                                                                           
                                                                                
    {                                                                           
        if (find_bifunc(env, to_symbol(fnc)); env.t_findbifunc_buf.first) {
            env.t_bi_evaler.push({
                env.t_findbifunc_buf.second.first,
                env.t_findbifunc_buf.second.second,
                args_beg_it,
                args_end_it,
                forse_nospread_args
            });
            return env.t_bi_evaler.pop_eval();
        }                                                                       
    }                                                                           
                                                                                
    if (auto lambdaopt = env.t_lambdas.find(to_symbol(fnc))) {
        env.t_l_evaler.push({ lambdaopt->get(),args_beg_it,args_end_it,forse_nospread_args });                                                               
        return env.t_l_evaler.pop_eval();
    }                                                                           
    else {                                                                      
        throw "eval_fnc error " + to_string(make_list({ fnc, make_list({args_beg_it , args_end_it}) }));                                                                      
    }                                                                           
}           

__forceinline std::optional<Cell> eval_implicit_cond(CoreEnvironment& env, const Cell& atom) {
    auto predicate = env.eval_quote(to_list(atom)[0]);
    if (!is_list(predicate) || (is_list(predicate) && !to_list(predicate).empty())) {
        if (to_list(atom).size() == 1) return env.t_symbols.nil;
        return env_eval_direct_bifunc(&CoreEnvironment::nbifunc_progn, next(begin(to_list(atom))), end(to_list(atom)), (&env));
    }
    return nullopt;
}

__forceinline Cell eval_atom(CoreEnvironment& env, const Atom& atom) {
    if (is_symbol(atom)) {
        return eval_symbol(env, to_symbol(atom));
    }
    return make_atom(atom);
}

__forceinline  Cell eval_symbol(CoreEnvironment& env, const Symbol& symbol) {
    if (is_null_symbol(symbol, env.t_symbols)) {
        return env.t_symbols.nil;
    }
    if (auto val = env.t_envs.get(symbol)) {
        return *val;
    }
    return make_symbol_cell(symbol);
}

__forceinline  void find_bifunc(CoreEnvironment& env, const Symbol& str) {
    if (auto it = env.t_symbols.bifuncs.find(str); it != end(env.t_symbols.bifuncs) && (it->first == str)) {
        env.t_findbifunc_buf = { true, {bifunc_type::bifunc,  it->second} };
    }
    else if (auto it = env.t_symbols.nbifuncs.find(str); it != end(env.t_symbols.nbifuncs) && (it->first == str)) {
        env.t_findbifunc_buf = { true, {bifunc_type::nbifunc,  it->second} };
    }
    else {
        env.t_findbifunc_buf.first = false;
    }
}





CoreEnvironment::CoreEnvironment(CoreEnvStreamsProvider& streams) :
    t_l_evaler(this),
    t_bi_evaler(this),
    t_streams(streams),
    t_syntaxer(t_symbols.symbols),
    t_direct_call_buf(t_symbols.nil),
    t_findbifunc_buf(false, {bifunc_type::bifunc, nullptr})//,
    //t_eval_atom_evaler(this)
{
	//t_envs.add_global_var(t_symbols.T_symbol, t_symbols.T);
}


std::vector<Cell> CoreEnvironment::execute_all(CoreInputStreamInt& stream, const IMutexed<bool>& stop_flag)
{
    t_stop_flag = { stop_flag };
    try
    {
        std::vector<Cell> result;
        auto s = t_syntaxer.read_sexprs_stream(stream);
        while (s.ready())
        {
            auto [reason, c] =  s.read();
            if (reason != read_reason::success) throw "input error";
            result.emplace_back(eval_quote(c));
        }
        return result;
    }
    catch (...)
    {
        t_envs.clear_subenvs();
        t_args.clear();
        t_l_evaler.clear();
        t_bi_evaler.clear();
        throw;
    }
}

Cell CoreEnvironment::execute_one(CoreInputStreamInt& stream, const IMutexed<bool>& stop_flag)
{
    t_stop_flag = { stop_flag };
    auto [read_reason, c] = t_syntaxer.read_sexpr(stream);
    if (read_reason != read_reason::success) throw "input error";
    try
    {
        return eval_quote(c);
    }
    catch (...)
    {
        t_envs.clear_subenvs();
        t_args.clear();
        t_l_evaler.clear();
        t_bi_evaler.clear();
        throw;
    }
}

//const std::unordered_map<Symbol, LambdaCell>& CoreEnvironment::lambdas() const
//{
//    return t_lambdas;
//}

const CellEnvironment::mp& CoreEnvironment::vars() const
{
    return t_envs.get_globals();
}

Cell CoreEnvironment::bifunc_atomp() {
    if (t_args.size() == 0) return t_symbols.T;
    if (is_list(arg1) && to_list(arg1).empty()) return t_symbols.T;
    return bool_cast(is_atom(arg1), t_symbols);
}

Cell CoreEnvironment::bifunc_symbolp() {
    if (t_args.size() == 0) return t_symbols.T;
    if (is_list(arg1) && to_list(arg1).empty()) return t_symbols.T;
    return bool_cast(is_symbol(arg1), t_symbols);
}

Cell CoreEnvironment::bifunc_numberp() {
    if (t_args.size() == 0) return t_symbols.nil;
    return bool_cast(is_number((arg1)), t_symbols);
}

Cell CoreEnvironment::bifunc_listp() {
    if (t_args.size() == 0) return t_symbols.T;
    return bool_cast(is_list(arg1), t_symbols);
}

Cell CoreEnvironment::bifunc_null() {
    if (t_args.size() == 0) return t_symbols.T;
    return bool_cast(is_null((arg1), t_symbols), t_symbols);
}

Cell CoreEnvironment::bifunc_add()
{
    if (t_args.size() == 0) return make_number(Number(BigInt(0l)));
    if (!is_number(arg1)) throw "bifunc_add error";
    Number n = to_number(arg1);

    for (auto it = begin(t_args) + 1; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_add error";
        n += to_number(*it);
    }
    return make_number(move(n));
}

Cell CoreEnvironment::bifunc_sub()
{
    if (t_args.size() == 0) throw "bifunc_sub error";
    if (t_args.size() == 1) {
        if (!is_number(arg1)) throw "bifunc_sub error";
        Number n = to_number(arg1);
        n.minus();
        return  make_number(move(n));
    }
    if (!is_number(arg1)) throw "bifunc_sub error";
    Number n = to_number(arg1);

    for (auto it = begin(t_args) + 1; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_sub error";
        n -= to_number(*it);
    }
    return make_number(move(n));
}

Cell CoreEnvironment::bifunc_mul()
{
    if (t_args.size() == 0) {
        return make_number(Number(BigInt(1l)));
    }
    if (!is_number(arg1)) throw "bifunc_mul error";
    if (t_args.size() == 1) {
        return arg1;
    }
    Number n = to_number(arg1);

    for (auto it = begin(t_args) + 1; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_mul error";
        n *= to_number(*it);
    }
    return make_number(move(n));
}

Cell CoreEnvironment::bifunc_div()
{
    if (t_args.size() == 0) throw "bifunc_div error";
    if (t_args.size() == 1) {
        if (!is_number(arg1)) throw "bifunc_div error";
        Number n(1);
        n /= to_number(arg1);
        return make_number(move(n));
    }

    if (!is_number(arg1)) throw "bifunc_div error";
    Number n = to_number(arg1);

    if (!is_number(arg2)) throw "bifunc_div error";
    Number n2 = to_number(arg2);

    for (auto it = begin(t_args) + 2; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_div error";
        n2 *= to_number(*it);
    }

    if (n2 == Number(BigInt(0l))) {
        throw "bifunc_div error (Zero Divide)";
    }

    n /= n2;

    return make_number(move(n));
}

Cell CoreEnvironment::bifunc_greater()
{
    if (t_args.size() == 0) return t_symbols.T;
    if (t_args.size() == 1) return t_symbols.T;
    if (!is_number(arg1)) throw "bifunc_greater error";
    Number n = to_number(arg1);

    bool result = true;
    auto it = begin(t_args) + 1;
    for (; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_greater error";
        Number n2 = to_number(*it);
        result = n > n2;

        if (!result) break;
        n = move(n2);
    }
    return bool_cast(result, t_symbols);
}

Cell CoreEnvironment::bifunc_greater_equal()
{
    if (t_args.size() == 0) return t_symbols.T;
    if (t_args.size() == 1) return t_symbols.T;
    if (!is_number(arg1)) throw "bifunc_greater_equal error";
    Number n = to_number(arg1);

    bool result = true;
    auto it = begin(t_args) + 1;
    for (; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_greater_equal error";
        Number n2 = to_number(*it);
        result = n >= n2;

        if (!result) break;
        n = move(n2);
    }
    return bool_cast(result, t_symbols);
}

Cell CoreEnvironment::bifunc_less()
{
    if (t_args.size() == 0) return t_symbols.T;
    if (t_args.size() == 1) return t_symbols.T;
    if (!is_number(arg1)) throw "bifunc_less error";
    Number n = to_number(arg1);

    bool result = true;
    auto it = begin(t_args) + 1;
    for (; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_less error";
        Number n2 = to_number(*it);
        result = n < n2;

        if (!result) break;
        n = move(n2);
    }

    return bool_cast(result, t_symbols);
}

Cell CoreEnvironment::bifunc_less_equal()
{
    if (t_args.size() == 0) return t_symbols.T;
    if (t_args.size() == 1) return t_symbols.T;
    if (!is_number(arg1)) throw "bifunc_less_equal error";
    Number n = to_number(arg1);

    bool result = true;
    auto it = begin(t_args) + 1;
    for (; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_less_equal error";
        Number n2 = to_number(*it);
        result = n <= n2;

        if (!result) break;
        n = move(n2);
    }

    return bool_cast(result, t_symbols);
}

Cell CoreEnvironment::bifunc_num_eq()
{
    if (t_args.size() == 0) return t_symbols.T;
    if (t_args.size() == 1) return t_symbols.T;
    if (!is_number(arg1)) throw "bifunc_num_eq error";
    Number n = to_number(arg1);

    bool result = true;
    auto it = begin(t_args) + 1;
    for (; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_num_eq error";
        Number n2 = to_number(*it);
        result = n == n2;

        if (!result) break;
        n = move(n2);
    }

    return bool_cast(result, t_symbols);
}

Cell CoreEnvironment::bifunc_equal()
{
    if (t_args.size() == 0) return t_symbols.T;
    if (t_args.size() == 1) return t_symbols.T;
    if (is_number(arg1) && is_number(arg2)) {
        return eval_direct_bifunc(&CoreEnvironment::bifunc_num_eq, begin(t_args), end(t_args));
    }

    if (is_symbol(arg1) && is_symbol(arg2)) {
        return eval_direct_bifunc(&CoreEnvironment::bifunc_eq, begin(t_args), end(t_args));
    }

    if (is_list(arg1) && is_list(arg2)) {
        const auto& s1 = to_list(arg1);
        const auto& s2 = to_list(arg2);

        if (s1.size() == s2.size()) {
            for (size_t i = 0; i < s1.size(); ++i) {
                DPair lst = { s1[i], s2[i] };
                
                if (
                    is_list(eval_direct_bifunc(&CoreEnvironment::bifunc_equal, begin(lst), end(lst)))
                    )
                {
                    return t_symbols.nil;
                }
            }
            return t_symbols.T;
        }  
    }

    return t_symbols.nil;
}

Cell CoreEnvironment::bifunc_car()
{
    Cell s = (t_args.size() >= 1) ? arg1 : t_symbols.nil;

    if (is_atom(s)) {
        return eval_atom(*this,to_atom(s));
    }
    else if (is_list(s)) {
        const auto& lst = to_list(s);
        if (lst.empty()) {
            return t_symbols.nil;
        }
        else {
            return lst[0];
        }
    }
    else {
        throw "bifunc_car error (dot pair not available)";
    }
}

Cell CoreEnvironment::bifunc_cdr()
{
    Cell s = (t_args.size() >= 1) ? arg1 : t_symbols.nil;

    if (is_atom(s)) {
        throw "bifunc_cdr error (property not available)";
    }
    else if (is_list(s)) {
        const auto& lst = to_list(s);
        if (lst.empty()) {
            return t_symbols.nil;
        }
        else {

            /// ?
            Cell result = make_list(lst);
            to_list(result).erase(begin(to_list(result)));
            return result;
        }
    }
    else {
        throw "bifunc_cdr error (dot pair not available)";
    }
}

Cell CoreEnvironment::bifunc_append()
{
    if (t_args.size() == 0) return t_symbols.nil;
    auto it = begin(t_args);
    while ((is_null(*it, t_symbols) || is_atom(*it)) && it != end(t_args)) {
        ++it;
    }
    if(it == end(t_args))return t_symbols.nil;
    if(it + 1 == end(t_args))return *it;
    Cell s = make_list(to_list(*it));
    auto& lst = to_list(s);
    ++it;
    for(; it != end(t_args); ++it){
        
        if (is_list(*it)) {
            if (is_null(*it, t_symbols)) continue;
            const auto& next_lst = to_list(*it);
            for (auto it2 = begin(next_lst); it2 != end(next_lst);++it2){
                lst.push_back(*it2);
            }
        }
        
    }
    return s;
}

Cell CoreEnvironment::bifunc_cons()
{
    Cell s1 = (t_args.size() >= 1) ? arg1: t_symbols.nil;
    Cell s2 = (t_args.size() >= 2) ? arg2 : t_symbols.nil;

    if (is_atom(s2)) {
        throw "bifunc_cons error (dot pair is not available)";
    }

    DPair result = to_list(s2);
    if (result.empty()) {
        result = { s1 };
    }
    else {
        result.insert(begin(result), s1);
    }
    return make_list(move(result));
}

Cell CoreEnvironment::bifunc_list()
{
    return make_list({begin(t_args), end(t_args)});
}

Cell CoreEnvironment::bifunc_getd()
{
    if (t_args.size() == 0) return t_symbols.nil;
    if(!is_symbol(arg1)) throw "bifunc_getf error";

    if (find_bifunc(*this, to_symbol(arg1)); t_findbifunc_buf.first) {
        return t_symbols.nil;
    }
    else {
        if (auto lambdaopt = t_lambdas.find(to_symbol(arg1))) {
            return make_symbol_cell(to_string(lambdaopt->get()), t_symbols.symbols);
        }
        else {
            return t_symbols.nil;
        }
    }
}

Cell CoreEnvironment::bifunc_read() {
    // stop input?
    auto [reason, cell] = t_syntaxer.read_sexpr(t_streams.t_input_stream());
    if (reason == read_reason::success) { return cell; }
    else return t_symbols.nil;
    //if (t_stop_flag && (*t_stop_flag).get().get()) throw throw_stop_helper{};
    //if (!t_streams.t_input_stream().alive()) return t_symbols.nil;
}

Cell CoreEnvironment::bifunc_prog1()
{
    if (t_args.size() == 0) return t_symbols.nil;
    Cell result = arg1;
    eval_direct_bifunc(&CoreEnvironment::nbifunc_progn, begin(t_args) + 1, end(t_args));
    return result;
}

Cell CoreEnvironment::nbifunc_loop()
{
    auto it = begin(t_args);
    for(;;) {
        if(it == end(t_args)) it = begin(t_args);
        if (is_implicit_cond(*it, t_symbols)) {
            auto cellopt = eval_implicit_cond(*this, *it);
            if (cellopt) return *cellopt;
        }
        else {
            eval_quote(*it);
        }
        ++it;
    }
}

Cell CoreEnvironment::bifunc_print()
{
    if (t_args.size() == 0) {
        t_streams.t_output_stream().out_new_line(nil_str);
        return t_symbols.nil;
    }
    else {
        t_streams.t_output_stream().out_new_line(to_string(arg1));
        return arg1;
    }
}

Cell CoreEnvironment::nbifunc_quote()
{
    if (t_args.size() == 0) return t_symbols.nil;
    return arg1;
}

Cell CoreEnvironment::nbifunc_defun() {
    if (t_args.size() < 2)  return t_symbols.nil;
    if (!is_symbol(arg1)) return t_symbols.nil;
    const auto& name = to_symbol(arg1);
    if (is_list(arg2)) {
        const auto& lst = to_list(arg2);
        if (is_alambda_form(arg2, t_symbols))
        {
            t_lambdas.add(name, get_lambda_form(begin(lst), end(lst)));
        }
        else {
            t_lambdas.add(
                name,
                make_spread_lambda_form(
                    begin(lst),
                    end(lst),
                    begin(t_args) + 2,
                    end(t_args)
                )
            );
        }
    }
    else if(is_symbol(arg2)) {
        t_lambdas.add(
            name,
            make_nospread_lambda_form(
                lambda_types::lambda,
                to_symbol(arg2),
                begin(t_args) + 2,
                end(t_args)
            )
        );
    }
    else {
        throw "nbifunc_defun error";
    }
    return arg1;
}

Cell CoreEnvironment::eval_quote(const Cell& arg) {
    if ((t_stop_flag) && (*t_stop_flag).get().get()) throw throw_stop_helper{};
    if (is_list(arg)) {
        if (is_null(arg, t_symbols)) return t_symbols.nil;
        return eval_fnc(*this, to_list(arg)[0], begin(to_list(arg)) + 1, end(to_list(arg)), false);                                                
    }
    else {
        return eval_atom(*this, to_atom(arg));
    }
}

Cell CoreEnvironment::nbifunc_cond()
{
    for (auto it = begin(t_args); it != end(t_args); ++it) {
        const auto& elem = *it;
        if (!is_list(elem)) throw "bifunc_cond error";

        const auto& lst = to_list(elem);
        auto predicate_val = eval_quote(lst[0]);
        if (is_symbol(predicate_val) && to_symbol(predicate_val) != t_symbols.nil_symbol) {
            if (to_list(*it).size() == 1) return predicate_val;
            return eval_direct_bifunc(&CoreEnvironment::nbifunc_progn, next(begin(lst)), end(lst));
        }
    }
    return t_symbols.nil;
}

Cell CoreEnvironment::nbifunc_progn()
{
    if (t_args.size() == 0) return t_symbols.nil;
    for (auto it = begin(t_args); it != prev(end(t_args)); ++it) {
        if (is_implicit_cond(*it, t_symbols)) {
            auto cellopt = eval_implicit_cond(*this, *it);
            if (cellopt) return *cellopt;
        }
        else {
            eval_quote(*it);
        }
    }

    const auto& last = *prev(end(t_args));
    if (is_implicit_cond(last, t_symbols)) {
        auto cellopt = eval_implicit_cond(*this, last);
        if (cellopt) return *cellopt;
        throw "nbifunc_progn error -> implicit cond at the end, but t_symbols.nil";
    }
    else {
        return eval_quote(last);
    }
}

Cell CoreEnvironment::bifunc_set()
{
    if (t_args.size() == 0) return t_symbols.nil;
    const auto& val = (begin(t_args) + 1 == end(t_args)) ? t_symbols.nil : arg2;
    set_value(*this, arg1, val);
    return val;
}

Cell CoreEnvironment::bifunc_eq()
{
    if (t_args.size() == 0) return t_symbols.nil;
    if (t_args.size() == 1 && is_list(arg1) && to_list(arg1).empty()) return t_symbols.T;
    if (t_args.size() == 1) return t_symbols.nil;
    const auto& s1 = arg1;
    const auto& s2 = arg2;

    if (!is_atom(s1) || !is_atom(s2)) {
        return t_symbols.nil;
    }
    else if (is_number(s1) && is_number(s2)) {
        return bool_cast(to_number(s1) == to_number(s2), t_symbols);
    }
    else if (is_symbol(s1) && is_symbol(s2)) {
        return bool_cast(to_symbol(s1) == to_symbol(s2), t_symbols);
    }
    return t_symbols.nil;
}

Cell CoreEnvironment::bifunc_last()
{
    if (t_args.size() == 0) return t_symbols.nil;
    const auto& el = arg1;
    if (is_atom(el)) {
        return t_symbols.nil;
    }
    else if(is_list(el)){
        const auto& lst = to_list(el);
        if (lst.empty()) return t_symbols.nil;
        if (!is_list(lst.back()) && !is_atom(lst.back())) {
            throw "bifunc_last error (dot pair not available)";
        }
        else {
            return make_list({ lst.back() });
        }
    } 
    else {
        throw "bifunc_last error (dot pair not available)";
    }
}

Cell CoreEnvironment::bifunc_length()
{
    if (t_args.size() == 0) return make_number(Number(BigInt(0l)));
    const auto& s = arg1;
    if (is_atom(s)) {
        throw "bifunc_length error (wtf ???)";
    }
    else if (is_list(s)) {
        return make_number(Number(BigInt(static_cast<long>(to_list(s).size()))));
    }
    else {
        throw "bifunc_length error (dot pair not available)";
    }
 }

Cell CoreEnvironment::bifunc_apply()
{
    if (t_args.size() == 0) {
        throw "bifunc_apply error (unknown function)";
    }

    if (t_args.size() >= 2) {
        if(is_list(arg2)) {
            return eval_fnc(*this, arg1, begin(to_list(arg2)), end(to_list(arg2)), true );
        }
        return eval_quote(make_list({ arg1 }));
    }
    else {
        return eval_quote(make_list({ arg1 }));
    }
}

Cell CoreEnvironment::nbifunc_setq()
{
    if (t_args.size() == 0) return t_symbols.nil;
    auto it = begin(t_args) + 1;
    auto val = (it == end(t_args)) ? t_symbols.nil : eval_quote(*it);

    set_value(*this, arg1, val);

    if (it != end(t_args) && (it + 1) != end(t_args)) {
        return eval_direct_bifunc(&CoreEnvironment::nbifunc_setq, it + 1, end(t_args));
    }
    else {
        return val;
    }
}

Cell CoreEnvironment::bifunc_eval() {
    if (t_args.size() == 0) return t_symbols.nil;
    return eval_quote(arg1);
}

Cell CoreEnvironment::bifunc_integerp()
{
    if (t_args.size() == 0) return t_symbols.nil;
    return bool_cast(is_int_number(arg1), t_symbols);
}

Cell CoreEnvironment::bifunc_oblist()
{
    const auto& lst = t_symbols.symbols.get_lst();
    DPair result;
    for (const auto& symb : lst) {
        result.emplace_back(make_symbol_cell(symb.data(), t_symbols.symbols));
    }
    return make_list(move(result));
}