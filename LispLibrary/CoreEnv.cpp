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

LambdaCell make_spread_lambda_form(
    lambda_types lambda_type,
    Cell::olist::const_iterator beg_params,
    Cell::olist::const_iterator end_params,
    Cell::olist::const_iterator beg_body,
    Cell::olist::const_iterator end_body
)
{
    auto size = std::distance(beg_params, end_params);
    std::vector<Symbol> params;
    params.reserve(size);
    for (auto it = beg_params; it != end_params; ++it) {
        if (!is_symbol(*it)) throw "get_lambda_form error";
        params.emplace_back(to_symbol(*it));
    }

    return  make_lambda(lambda_type, lambda_args_types::spread, move(params), { beg_body , end_body });
}

LambdaCell make_nospread_lambda_form(
    lambda_types lambda_type,
    Symbol param,
    Cell::olist::const_iterator beg_body,
    Cell::olist::const_iterator end_body
)
{
    return  make_lambda(lambda_type, lambda_args_types::nospread, { move(param) }, { beg_body , end_body });
}



LambdaCell get_lambda_form(
    Cell::olist::const_iterator beg_it,
    Cell::olist::const_iterator end_it
)
{
    if (std::distance(beg_it, end_it) < 2) throw "get_lambda_form error";
    if (!is_symbol(*beg_it)) throw "get_lambda_form error";
    if (
        to_symbol(*beg_it) == CoreData::nlambda_str
        ||
        to_symbol(*beg_it) == CoreData::lambda_str
        )
    {
        if (is_list(*(beg_it + 1))) {
            return make_spread_lambda_form(
                (to_symbol(*beg_it) == CoreData::lambda_str) ? lambda_types::lambda : lambda_types::nlambda,
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
    }
    throw "get_lambda_form error";
}

CoreEnvironment::CoreEnvironment(CoreEnvStreamsProvider& streams) :
    t_l_evaler(this),
    t_bi_evaler(this),
    t_streams(streams),
    t_syntaxer(t_symbols.symbols),
    t_direct_call_buf(t_symbols.nil)
{
	t_envs.add_global_var(t_symbols.T_symbol, t_symbols.T);
    //t_vars.emplace("*READ-UPCASE*", T);
}


std::vector<Cell> CoreEnvironment::execute_all(CoreInputStreamInt& stream, const IMutexed<bool>& stop_flag)
{
    t_stop_flag = { stop_flag };
    auto [read_reason, exprs] = t_syntaxer.read_sexprs(stream);
    if (read_reason != read_reason::success) throw "input error";
    try
    {
        std::vector<Cell> result;
        for (const auto&  c: exprs) {
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

const std::unordered_map<Symbol, LambdaCell>& CoreEnvironment::lambdas() const
{
    return t_lambdas;
}

const CellEnvironment::mp& CoreEnvironment::vars() const
{
    return t_envs.get_globals();
}

Cell CoreEnvironment::bifunc_atomp() {
    if (t_args.size() == 0) return t_symbols.T;
    if (is_list(*begin(t_args)) && to_list(*begin(t_args)).empty()) return t_symbols.T;
    return bool_cast(is_atom(*begin(t_args)), t_symbols);
}

Cell CoreEnvironment::bifunc_symbolp() {
    if (t_args.size() == 0) return t_symbols.T;
    if (is_list(*begin(t_args)) && to_list(*begin(t_args)).empty()) return t_symbols.T;
    return bool_cast(is_symbol(*begin(t_args)), t_symbols);
}

Cell CoreEnvironment::bifunc_numberp() {
    if (t_args.size() == 0) return t_symbols.nil;
    return bool_cast(is_number((*begin(t_args))), t_symbols);
}

Cell CoreEnvironment::bifunc_listp() {
    if (t_args.size() == 0) return t_symbols.T;
    return bool_cast(is_list(*begin(t_args)), t_symbols);
}

Cell CoreEnvironment::bifunc_null() {
    if (t_args.size() == 0) return t_symbols.T;
    return bool_cast(is_null((*begin(t_args)), t_symbols), t_symbols);
}

Cell CoreEnvironment::bifunc_add()
{
    if (t_args.size() == 0) return make_number(Number(BigInt(0l)));
    if (!is_number(*begin(t_args))) throw "bifunc_add error";
    Number n = to_number(*begin(t_args));

    for (auto it = begin(t_args) + 1; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_add error";
        n += to_number(*it);
    }
    return make_number(n);
}

Cell CoreEnvironment::bifunc_sub()
{
    if (t_args.size() == 0) throw "bifunc_sub error";
    if (t_args.size() == 1) {
        if (!is_number(*begin(t_args))) throw "bifunc_sub error";
        Number n = to_number(*begin(t_args));
        n.minus();
        return  make_number(n);
    }
    if (!is_number(*begin(t_args))) throw "bifunc_sub error";
    Number n = to_number(*begin(t_args));

    for (auto it = begin(t_args) + 1; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_sub error";
        n -= to_number(*it);
    }
    return  make_number(n);
}

Cell CoreEnvironment::bifunc_mul()
{
    if (t_args.size() == 0) {
        return make_number(Number(BigInt(1l)));
    }
    if (!is_number(*begin(t_args))) throw "bifunc_mul error";
    if (t_args.size() == 1) {
        return *begin(t_args);
    }
    Number n = to_number(*begin(t_args));

    for (auto it = begin(t_args) + 1; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_mul error";
        n *= to_number(*it);
    }
    return  make_number(n);
}

Cell CoreEnvironment::bifunc_div()
{
    if (t_args.size() == 0) throw "bifunc_div error";
    if (t_args.size() == 1) {
        if (!is_number(*begin(t_args))) throw "bifunc_div error";
        Number n(1);
        n /= to_number(*begin(t_args));
        return  make_number(n);
    }

    if (!is_number(*begin(t_args))) throw "bifunc_div error";
    Number n = to_number(*begin(t_args));

    if (!is_number(*(begin(t_args) + 1))) throw "bifunc_div error";
    Number n2 = to_number(*(begin(t_args) + 1));

    for (auto it = begin(t_args) + 2; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_div error";
        n2 *= to_number(*it);
    }

    if (n2 == Number(BigInt(0l))) {
        throw "bifunc_div error (Zero Divide)";
    }

    n /= n2;

    return  make_number(n);
}

Cell CoreEnvironment::bifunc_greater()
{
    if (t_args.size() == 0) return t_symbols.T;
    if (t_args.size() == 1) return t_symbols.T;
    if (!is_number(*begin(t_args))) throw "bifunc_greater error";
    Number n = to_number(*begin(t_args));

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
    if (!is_number(*begin(t_args))) throw "bifunc_greater_equal error";
    Number n = to_number(*begin(t_args));

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
    if (!is_number(*begin(t_args))) throw "bifunc_less error";
    Number n = to_number(*begin(t_args));

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
    if (!is_number(*begin(t_args))) throw "bifunc_less_equal error";
    Number n = to_number(*begin(t_args));

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
    if (!is_number(*begin(t_args))) throw "bifunc_num_eq error";
    Number n = to_number(*begin(t_args));

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
    if (is_number(*begin(t_args)) && is_number(*(begin(t_args) + 1))) {
        return eval_direct_bifunc(&CoreEnvironment::bifunc_num_eq, begin(t_args), end(t_args));
    }

    if (is_symbol(*begin(t_args)) && is_symbol(*(begin(t_args) + 1))) {
        return eval_direct_bifunc(&CoreEnvironment::bifunc_eq, begin(t_args), end(t_args));
    }

    if (is_list(*begin(t_args)) && is_list(*(begin(t_args) + 1))) {
        const auto& s1 = to_list(*begin(t_args));
        const auto& s2 = to_list(*(begin(t_args) + 1));

        if (s1.size() == s2.size()) {
            for (size_t i = 0; i < s1.size(); ++i) {
                Cell::olist lst = { s1[i], s2[i] };
                
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
    Cell s = (t_args.size() >= 1) ? *begin(t_args) : t_symbols.nil;

    if (is_atom(s)) {
        return eval_quote(s);
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
    Cell s = (t_args.size() >= 1) ? *begin(t_args) : t_symbols.nil;

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
    Cell s1 = (t_args.size() >= 1) ? *begin(t_args): t_symbols.nil;
    Cell s2 = (t_args.size() >= 2) ? *(begin(t_args) + 1) : t_symbols.nil;

    if (is_atom(s2)) {
        throw "bifunc_cons error (dot pair is not available)";
    }

    Cell::olist result = to_list(s2);
    if (result.empty()) {
        result = { s1 };
    }
    else {
        result.insert(begin(result), s1);
    }
    return make_list(std::move(result));
}

Cell CoreEnvironment::bifunc_list()
{
    return make_list({begin(t_args), end(t_args)});
}

Cell CoreEnvironment::bifunc_getd()
{
    if (t_args.size() == 0) return t_symbols.nil;
    if(!is_symbol(*begin(t_args))) throw "bifunc_getf error";

    if (find_bifunc(to_symbol(*begin(t_args))).first != find_bifunc_result::null) {
        return t_symbols.nil;
    }
    else {
        if (t_lambdas.find(to_symbol(*begin(t_args))) != end(t_lambdas)) {
            return make_symbol_cell(to_string(t_lambdas.at(to_symbol(*begin(t_args)))), t_symbols.symbols);
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
    Cell result = *begin(t_args);
    eval_direct_bifunc(&CoreEnvironment::nbifunc_progn, begin(t_args) + 1, end(t_args));
    return result;
}

Cell CoreEnvironment::nbifunc_loop()
{
    auto it = begin(t_args);
    for(;;) {
        if(it == end(t_args)) it = begin(t_args);
        if (is_implicit_cond(*it, t_symbols)) {
            auto cellopt = eval_implicit_cond(*it);
            if (cellopt) return *cellopt;
        }
        else {
            eval_quote(*it);
        }
        ++it;
    }
    throw "CoreEnvironment::nbifunc_loop: ???";
}

Cell CoreEnvironment::bifunc_print()
{
    if (t_args.size() == 0) {
        t_streams.t_output_stream().out_new_line(nil_str);
        return t_symbols.nil;
    }
    else {
        t_streams.t_output_stream().out_new_line(to_string(*begin(t_args)));
        return *begin(t_args);
    }
}

Cell CoreEnvironment::nbifunc_quote()
{
    if (t_args.size() == 0) return t_symbols.nil;
    return *begin(t_args);
}

Cell CoreEnvironment::nbifunc_defun() {
    if (t_args.size() < 2)  return t_symbols.nil;
    if (!is_symbol(*begin(t_args))) return t_symbols.nil;
    const auto& name = to_symbol(*begin(t_args));
    if (is_list(*(begin(t_args) + 1))) {
        const auto& lst = to_list(*(begin(t_args) + 1));
        if (
                !lst.empty()
                && is_symbol(lst[0])
                && 
                (to_symbol(lst[0]) == lambda_str || to_symbol(lst[0]) == nlambda_str)
            ) 
        {
            t_lambdas[name] = get_lambda_form(begin(lst), end(lst));
        }
        else {
            t_lambdas[name] =
                make_spread_lambda_form(
                    lambda_types::lambda,
                    begin(lst),
                    end(lst),
                    begin(t_args) + 2,
                    end(t_args)
                );
        }
    }
    else if(is_symbol(*(begin(t_args) + 1))) {
        t_lambdas[name] =
            make_nospread_lambda_form(
                lambda_types::lambda,
                to_symbol(*(begin(t_args) + 1)),
                begin(t_args) + 2,
                end(t_args)
            );
    }
    else {
        throw "nbifunc_defun error";
    }
    return *begin(t_args);
}

std::optional<Cell> CoreEnvironment::eval_implicit_cond(const Cell& atom) {
    auto predicate = eval_quote(to_list(atom)[0]);
    if (!is_list(predicate) || (is_list(predicate) && !to_list(predicate).empty())) {
        if (to_list(atom).size() == 1) return t_symbols.nil;
        return eval_direct_bifunc(&CoreEnvironment::nbifunc_progn, next(begin(to_list(atom))), end(to_list(atom)));
    }
    return nullopt;
}


Cell CoreEnvironment::eval_atom(const Cell& atom) {
    if (is_null(atom, t_symbols)) {
        return atom;
    }
    if (is_symbol(atom)) {
        const auto& name = to_symbol(atom);
        auto val = t_envs.get(name);
        if (val) {
            return val->get();
        }
    }
    return atom;
}

std::pair<CoreEnvironment::find_bifunc_result, CoreData::bifunc> CoreEnvironment::find_bifunc(const Symbol& str) {
    {
        auto it =
            std::lower_bound(
                begin(t_symbols.bifuncs),
                end(t_symbols.bifuncs),
                str,
                bifunc_pair_less_w_string{}
            );
        if (it != end(t_symbols.bifuncs) && (it->first == str)) {
            return { find_bifunc_result::bi,  it->second };
        }
    }
    {
        auto it =
            std::lower_bound(
                begin(t_symbols.nbifuncs),
                end(t_symbols.nbifuncs),
                str,
                bifunc_pair_less_w_string{}
            );
        if (it != end(t_symbols.nbifuncs) && (it->first == str)) {
            return { find_bifunc_result::nbi,  it->second };
        }
    }

    return { find_bifunc_result::null, nullptr};
}

Cell CoreEnvironment::eval_quote(const Cell& arg) {
    if ((t_stop_flag) && (*t_stop_flag).get().get()) throw throw_stop_helper{};
    if (is_list(arg)) {
        const auto& lst = to_list(arg);
        if (lst.empty()) return t_symbols.nil;
        if (!is_symbol(lst[0])) {
            if (is_list(lst[0]))
            {
                auto lambda = get_lambda_form(begin(to_list(lst[0])), end(to_list(lst[0])));
                if (lambda.is_lambda()) {
                    t_l_evaler.push_lambda(lambda, begin(lst) + 1, end(lst), true);
                    return t_l_evaler.pop_eval();
                }
                else if (lambda.is_nlambda()) {
                    t_l_evaler.push_lambda(lambda, begin(lst) + 1, end(lst));
                    return t_l_evaler.pop_eval();
                }
                else {
                    throw "eval error";
                }
            }

            throw "eval error";
        }

        {
            bool bifunc = false;
            {
                auto [reason, fnc] = find_bifunc(to_symbol(lst[0]));
                if (reason == find_bifunc_result::bi) {
                    t_bi_evaler.push_bifunc(fnc, begin(lst) + 1, end(lst), true);
                    bifunc = true;
                }
                else if (reason == find_bifunc_result::nbi) {
                    t_bi_evaler.push_nbifunc(fnc, begin(lst) + 1, end(lst));
                    bifunc = true;
                }
            }
            if (bifunc) {
                return t_bi_evaler.pop_eval();
            }
        }

        if (t_lambdas.find(to_symbol(lst[0])) != end(t_lambdas)) {
            const auto& lambda = t_lambdas.at(to_symbol(lst[0]));
            if (lambda.is_lambda()) {
                t_l_evaler.push_lambda(lambda, begin(lst) + 1, end(lst), true);
                return t_l_evaler.pop_eval();
            }
            else if (lambda.is_nlambda()) {
                t_l_evaler.push_lambda(lambda, begin(lst) + 1, end(lst));
                return t_l_evaler.pop_eval();
            }
            else {
                throw "eval error";
            }
        }
        else {
            throw "eval error";
        }
    }
    else {
        return eval_atom(arg);
    }
}

std::vector<Cell> CoreEnvironment::eval_args(Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it) {
    std::vector<Cell> result;
    for (Cell::olist::const_iterator it = beg_it; it != end_it; ++it) {
        result.emplace_back(eval_quote(*it));
    }
    return result;
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
            auto cellopt = eval_implicit_cond(*it);
            if (cellopt) return *cellopt;
        }
        else {
            eval_quote(*it);
        }
    }

    auto it = prev(end(t_args));
    if (is_implicit_cond(*it, t_symbols)) {
        auto cellopt = eval_implicit_cond(*it);
        if (cellopt) return *cellopt;
        throw "nbifunc_progn error -> implicit cond at the end, but t_symbols.nil";
    }
    else {
        return eval_quote(*it);
    }
}

Cell CoreEnvironment::bifunc_set()
{
    if (t_args.size() == 0) return t_symbols.nil;
    const auto& name_cell = *begin(t_args);
    if (!is_symbol(name_cell)) throw "bifunc_set error";
    const auto& name = to_symbol(name_cell);
    auto it = begin(t_args) + 1;
    const auto& val = (it == end(t_args)) ? t_symbols.nil : *it;

    if (auto var = t_envs.get(name)) {
        var->get() = val;
    }
    else {
        t_envs.add_global_var(name, val);
    }

    if (it != end(t_args) && (it + 1) != end(t_args)) {
        return eval_direct_bifunc(&CoreEnvironment::bifunc_set, it, end(t_args));
    }
    else {
        return val;
    }
}

Cell CoreEnvironment::bifunc_eq()
{
    if (t_args.size() == 0) return t_symbols.nil;
    if (t_args.size() == 1 && is_list(*(begin(t_args))) && to_list(*(begin(t_args))).empty()) return t_symbols.T;
    if (t_args.size() == 1) return t_symbols.nil;
    const auto& s1 = *(begin(t_args));
    const auto& s2 = *(begin(t_args) + 1);

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
    const auto& el = *(begin(t_args));
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
    const auto& s = *(begin(t_args));
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
    const auto& fnc = (*begin(t_args));
    if (t_args.size() >= 2) {
        const auto& lst_cell = *(begin(t_args) + 1);
        if(is_list(lst_cell)) {
            const auto& lst = to_list(lst_cell);

            if (!is_symbol(fnc)) {
                if (is_list(fnc))
                {
                    auto lambda = get_lambda_form(begin(to_list(fnc)), end(to_list(fnc)));
                    
                    if (lambda.is_lambda()) {
                        t_l_evaler.push_lambda(lambda, begin(lst), end(lst), false);
                        return t_l_evaler.pop_eval();
                    }
                    else if (lambda.is_nlambda()) {
                        t_l_evaler.push_lambda(lambda, begin(lst), end(lst));
                        return t_l_evaler.pop_eval();
                    }
                    else {
                        throw "eval error";
                    }
                }

                throw "eval error";
            }

            {
                auto [reason, f] = find_bifunc(to_symbol(fnc));
                if (reason == find_bifunc_result::bi) {
                    t_bi_evaler.push_bifunc(f, begin(lst), end(lst), false);
                    return t_bi_evaler.pop_eval();
                }
                else if (reason == find_bifunc_result::nbi) {
                    t_bi_evaler.push_nbifunc(f, begin(lst), end(lst));
                    return t_bi_evaler.pop_eval();
                }
            }


        }
        else {
            return eval_quote(make_list({ fnc }));
        }
    }
    else {
        return eval_quote(make_list({fnc}));
    }
}

Cell CoreEnvironment::nbifunc_setq()
{
    if (t_args.size() == 0) return t_symbols.nil;
    if (!is_symbol(*(begin(t_args)))) throw "bifunc_setq error";
    const auto& name = to_symbol(*(begin(t_args)));

    auto it = begin(t_args) + 1;
    auto val = (it == end(t_args)) ? t_symbols.nil : eval_quote(*it);

    if (auto var = t_envs.get(name)) {
        var->get() = val;
    }
    else {
        t_envs.add_global_var(name, val);
    }


    //section for special system vars
    if (name == t_symbols.read_up_case_symbol) {
        if (is_null(val, t_symbols)) {
            t_syntaxer.set_upcase_mode(false);
        }
    }

    if (it != end(t_args) && (it + 1) != end(t_args)) {
        return eval_direct_bifunc(&CoreEnvironment::bifunc_set, it, end(t_args));
    }
    else {
        return val;
    }
}

Cell CoreEnvironment::bifunc_eval() {
    if (t_args.size() == 0) return t_symbols.nil;
    return eval_quote(*begin(t_args));
}

Cell CoreEnvironment::bifunc_integerp()
{
    if (t_args.size() == 0) return t_symbols.nil;
    return bool_cast(is_int_number(*begin(t_args)), t_symbols);
}

Cell CoreEnvironment::bifunc_oblist()
{
    const auto& lst = t_symbols.symbols.get_lst();
    Cell::olist result;
    for (const auto& symb : lst) {
        result.emplace_back(make_symbol_cell(symb.data(), t_symbols.symbols));
    }
    return make_list(move(result));
}
