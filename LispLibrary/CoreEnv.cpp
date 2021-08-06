#include "CoreEnv.h"
#include "Funcs.h"
#include "BigInt.h"
#include "BIFuncEvaler.h"
#include "Lexer.h"
#include "DotPair.h"
#include "CarCdrIterator.h"


#include <list>
#include <iostream>
#include <unordered_map>
#include <variant>
#include <cmath>
#include <algorithm>
#include <sstream>

#include "test_runner.h"

using namespace std;
using namespace CoreData;

#define arg1 *begin(t_args)
#define arg2 *(next(begin(t_args), 1))
#define arg3 *(next(begin(t_args), 2))


__forceinline lambda make_spread_lambda_form(
    lambda_types lambda_type,
    const DotPair& params,
    const DotPair& body
)
{
    std::vector<Symbol> params_v;
    for (const auto& cell : CarCdrConstIteration( params )) {
        if (!is_symbol(cell)) throw "get_lambda_form error";
        params_v.emplace_back(to_symbol(cell));
    }

    return  make_lambda(lambda_type, lambda_args_types::spread, move(params_v), body);
}

__forceinline lambda make_nospread_lambda_form(
    lambda_types lambda_type,
    const Symbol& param,
    const DotPair& body
)
{
    return  make_lambda(lambda_type, lambda_args_types::nospread, { param }, body);
}

__forceinline lambda get_lambda_form(
    CoreEnvironment& env,
    const DotPair& lst
)
{
    if (is_null(cdr(lst), env.t_symbols)) throw "get_lambda_form error";
    const auto& second = car(cdr(lst));
    if (is_list(second)) {
        return make_spread_lambda_form(
            (to_symbol(car(lst)) == CoreData::lambda_str) ? lambda_types::lambda : lambda_types::nlambda,
            to_list(second),
            to_list(cdr(cdr(lst)))
        );
    }
    else if(is_symbol(second)){
        return make_nospread_lambda_form(
            (to_symbol(car(lst)) == CoreData::lambda_str) ? lambda_types::lambda : lambda_types::nlambda,
            to_symbol(second),
            to_list(cdr(cdr(lst)))
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
    vector<Cell>::const_iterator args_beg_it,
    vector<Cell>::const_iterator args_end_it,
    bool forse_nospread_args
)
{                                                                               
    if (is_alambda_form(fnc, env.t_symbols)) {
        auto l = get_lambda_form(env, to_list(fnc));
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
        throw "eval_fnc error " + to_string(
            make_list({ fnc, make_list_cell(args_beg_it , args_end_it, env.t_symbols) }, env.t_symbols)
        );
    }                                                                           
}           

__forceinline std::optional<Cell> eval_implicit_cond(CoreEnvironment& env, const Cell& atom) {
    auto predicate = env.eval_quote(car(to_list(atom)));
    if (!is_list(predicate) || (is_list(predicate) && !empty(to_list(predicate)))) {
        if (
            const auto& cdr_buf = cdr(to_list(atom));
            is_symbol(cdr_buf) && to_symbol(cdr_buf) == env.t_symbols.nil_symbol
            )
        {
            return env.t_symbols.nil;
        }
        vector<Cell> lst_buf = copy_dpair_list_to_vector(cdr(to_list(atom)));
        return env_eval_direct_bifunc(&CoreEnvironment::nbifunc_progn, begin(lst_buf), end(lst_buf), (&env));
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
    t_syntaxer(t_symbols),
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
            if (reason == read_reason::empty_input) {
                break;
            }
            if (reason != read_reason::success)  {
                throw "input error";
            }
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

    for (auto it = next(begin(t_args) , 1); it != end(t_args); ++it) {
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

    for (auto it = next(begin(t_args) , 1); it != end(t_args); ++it) {
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

    for (auto it = next(begin(t_args) , 1); it != end(t_args); ++it) {
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

    for (auto it = next(begin(t_args) , 2); it != end(t_args); ++it) {
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
    auto it = next(begin(t_args) , 1);
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
    auto it = next(begin(t_args) , 1);
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
    auto it = next(begin(t_args) , 1);
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
    auto it = next(begin(t_args) , 1);
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
    auto it = next(begin(t_args) , 1);
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
        reference_wrapper<const DotPair> s1 = to_list(arg1);
        reference_wrapper<const DotPair> s2 = to_list(arg2);
        
        while (!is_null(s1) && !is_null(s2)) 
        {
            std::vector<Cell> lst = { car(s1), car(s2) };
                
            if (
                is_null(eval_direct_bifunc(&CoreEnvironment::bifunc_equal, begin(lst), end(lst)), t_symbols)
                )
            {
                return t_symbols.nil;
            }
            s1 = to_list(cdr(s1));
            s2 = to_list(cdr(s2));
        } 
        if (is_null(s1) && is_null(s2)) return t_symbols.T;
        std::vector<Cell> lst = { car(s1), car(s2) };
        return bool_cast(!is_null(eval_direct_bifunc(&CoreEnvironment::bifunc_equal, begin(lst), end(lst)), t_symbols), t_symbols);
    }

    return t_symbols.nil;
}

Cell CoreEnvironment::bifunc_car()
{
    Cell s = (t_args.size() >= 1) ? arg1 : t_symbols.nil;

    if (is_atom(s)) {
        return eval_atom(*this,to_atom(s));
    }
    else {
        const auto& lst = to_list(s);
        if (lst.empty()) {
            return t_symbols.nil;
        }
        else {
            return car(lst);
        }
    }
}

Cell CoreEnvironment::bifunc_cdr()
{
    Cell s = (t_args.size() >= 1) ? arg1 : t_symbols.nil;

    if (is_atom(s)) {
        throw "bifunc_cdr error (property not available)";
    }
    else {
        const auto& lst = to_list(s);
        if (lst.empty()) {
            return t_symbols.nil;
        }
        else {
            return cdr(lst);
        }
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
    if(next(it, 1) == end(t_args))return *it;
    Cell s = *it;
    ++it;
    for(; it != end(t_args); ++it){
        s = append_cell(s, *it, t_symbols);
    }
    return s;
}

Cell CoreEnvironment::bifunc_cons()
{
    Cell s1 = (t_args.size() >= 1) ? arg1: t_symbols.nil;
    Cell s2 = (t_args.size() >= 2) ? arg2 : t_symbols.nil;

    return cons_cell(s1, s2, t_symbols);
}

Cell CoreEnvironment::bifunc_list()
{
    return make_list_cell(begin(t_args), end(t_args),t_symbols);
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
    eval_direct_bifunc(&CoreEnvironment::nbifunc_progn, next(begin(t_args) , 1), end(t_args));
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
        if (is_alambda_form(lst, t_symbols))
        {
            t_lambdas.add(name, get_lambda_form(*this, lst));
        }
        else {
            t_lambdas.add(
                name,
                make_spread_lambda_form(
                    lambda_types::lambda,
                    lst,
                    make_list(
                        next(begin(t_args) , 2),
                        end(t_args),
                        t_symbols
                    )
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
                make_list(
                    next(begin(t_args) , 2),
                    end(t_args),
                    t_symbols
                )
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
        //cout << to_string(cdr(to_list(arg))) << endl;
        auto lst_buf = copy_dpair_list_to_vector(cdr(to_list(arg)));
        //cout << lst_buf << endl;
        return eval_fnc(*this, car(to_list(arg)),  begin(lst_buf), end(lst_buf), false);
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
        auto predicate_val = eval_quote(car(lst));
        if (is_symbol(predicate_val) && to_symbol(predicate_val) != t_symbols.nil_symbol) {
            if (
                const auto& cdr_buf = cdr(to_list(*it));
                is_symbol(cdr_buf) && to_symbol(cdr_buf) == t_symbols.nil_symbol
            )
            {
                return predicate_val;
            }
            vector<Cell> lst_buf = copy_dpair_list_to_vector(cdr(lst));
            return eval_direct_bifunc(&CoreEnvironment::nbifunc_progn, begin(lst_buf), end(lst_buf));
        }
    }
    return t_symbols.nil;
}

Cell CoreEnvironment::nbifunc_progn()
{
    if (t_args.size() == 0) return t_symbols.nil;
    auto it = begin(t_args);
    for (; next(it) != end(t_args); ++it) {
        if (is_implicit_cond(*it, t_symbols)) {
            auto cellopt = eval_implicit_cond(*this, *it);
            if (cellopt) return *cellopt;
        }
        else {
            eval_quote(*it);
        }
    }

    const auto& last = *it;
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
    const auto& val = (next(begin(t_args) , 1) == end(t_args)) ? t_symbols.nil : arg2;
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
    else {
        if (is_null(to_list(el))) {
            return t_symbols.nil;
        }
        CarCdrConstIteration iteration(to_list(el));
        auto it = begin(iteration);
        while (next(it) != end(iteration)) {
            ++it;
        }
        return make_list_cell(it.get_pair());
    } 
}

Cell CoreEnvironment::bifunc_length()
{
    if (t_args.size() == 0) return make_number(Number(BigInt(0l)));
    const auto& s = arg1;
    if (is_atom(s)) {
        throw "bifunc_length error (wtf ???)";
    }
    else {
        CarCdrConstIteration iteration(to_list(s));
        return make_number(Number(BigInt(static_cast<long>(std::distance(begin(iteration), end(iteration))))));
    }
 }

Cell CoreEnvironment::bifunc_apply()
{
    if (t_args.size() == 0) {
        throw "bifunc_apply error (unknown function)";
    }

    if (t_args.size() >= 2) {
        if(is_list(arg2)) {
            auto lst_buf = copy_dpair_list_to_vector(to_list(arg2));
            return eval_fnc(*this, arg1, begin(lst_buf), end(lst_buf), true );
        }
        return eval_quote(make_list_cell({ arg1 }, t_symbols));
    }
    else {
        return eval_quote(make_list_cell({ arg1 }, t_symbols));
    }
}

Cell CoreEnvironment::nbifunc_setq()
{
    if (t_args.size() == 0) return t_symbols.nil;
    auto it = next(begin(t_args) , 1);
    auto val = (it == end(t_args)) ? t_symbols.nil : eval_quote(*it);

    set_value(*this, arg1, val);

    if (it != end(t_args) && next(it, 1) != end(t_args)) {
        return eval_direct_bifunc(&CoreEnvironment::nbifunc_setq, next(it, 1), end(t_args));
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
    std::list<Cell> a;
    std::vector <Cell> result;
    for (const auto& symb : lst) {
        result.push_back(make_symbol_cell(symb.data(), t_symbols.symbols));
    }
    return make_list_cell(begin(result), end(result), t_symbols);
}

Cell CoreEnvironment::bifunc_rplaca()
{
    Cell s1 = (t_args.size() >= 1) ? arg1 : t_symbols.nil;
    Cell s2 = (t_args.size() >= 2) ? arg2 : t_symbols.nil;

    if (!is_symbol(s1) && !is_null(to_list(s1))) {
        auto& c = car(s1);
        c = s2;
        return s1;
    }
    return t_symbols.nil;
}

Cell CoreEnvironment::bifunc_rplacd()
{
    Cell s1 = (t_args.size() >= 1) ? arg1 : t_symbols.nil;
    Cell s2 = (t_args.size() >= 2) ? arg2 : t_symbols.nil;

    if (!is_symbol(s1) && !is_null(to_list(s1))) {
        auto& c = cdr(s1);
        c = s2;
        return s1;
    }
    return t_symbols.nil;
}
