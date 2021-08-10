#include "CoreEnv.h"
#include "Funcs.h"
#include "CarCdrIterator.h"

#include "Funcs.h"

#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;
using namespace CoreData;

#define arg1 *begin(t_args)
#define arg2 *(next(begin(t_args), 1))
#define arg3 *(next(begin(t_args), 2))


lambda CoreEnvironment::make_spread_lambda_form(
    lambda_types lambda_type,
    Cell& params,
    Cell& body
)
{
    return  make_lambda(lambda_type, lambda_args_types::spread, tree_copy(params, t_farm), body);
}

lambda CoreEnvironment::make_nospread_lambda_form(
    lambda_types lambda_type,
    const Symbol& param,
    Cell& body
)
{
    return  make_lambda(lambda_type, lambda_args_types::nospread, t_farm.make_symbol_cell( param ), body);
}

lambda CoreEnvironment::get_lambda_form(Cell& lst)
{
    if (is_null(cdr(lst), t_farm)) throw "get_lambda_form error";
    auto& second = car(cdr(lst));
    if (is_list(second)) {
        return make_spread_lambda_form(
            (to_symbol(car(lst)) == CoreData::lambda_str) ? lambda_types::lambda : lambda_types::nlambda,
            second,
            cdr(cdr(lst))
        );
    }
    else if(is_symbol(second)){
        return make_nospread_lambda_form(
            (to_symbol(car(lst)) == CoreData::lambda_str) ? lambda_types::lambda : lambda_types::nlambda,
            to_symbol(second),
            cdr(cdr(lst))
        );
    }
    throw "get_lambda_form error";
}

Cell CoreEnvironment::numbers_compare(numbers_compare_type type)
{
    if (t_args == 0) return t_farm.T;
    if (t_args == 1) return t_farm.T;
    if (!is_number(arg1)) throw "numbers_compare: error";
    const auto* n = &to_number(arg1);

    bool result = true;
    auto it = next(begin(t_args));
    for (; it != end(t_args); ++it) {
        if (!is_number(*it)) throw "numbers_compare: error";
        const auto* n2 = &to_number(*it);

        switch (type)
        {
        case CoreEnvironment::numbers_compare_type::greater:
                result = *n > *n2;
            break;
        case CoreEnvironment::numbers_compare_type::greater_eq:
                result = *n >= *n2;
            break;
        case CoreEnvironment::numbers_compare_type::less:
                result = *n < *n2;
            break;
        case CoreEnvironment::numbers_compare_type::less_eq:
                result = *n <= *n2;
            break;
        case CoreEnvironment::numbers_compare_type::eq:
                result = *n == *n2;
            break;
        default:
            throw "numbers_compare: unknown compare";
            break;
        }

        if (!result) break;
        n = n2;
    }

    return bool_cast(result, t_farm);
}

void CoreEnvironment::set_value(const Cell& name, const Cell& val)
{
    if (!is_symbol(name)) {
        if (!is_null(name, t_farm)) {
            throw "bifunc_set error";
        }
    }
    else {
        const auto& symb = to_symbol(name);
        if (auto var = t_envs.get(symb)) {
            *var = val;
        }
        else {
            t_envs.add_global_var(symb, val);
        }
        if (symb == to_symbol(t_farm.read_up_case_symbol)) {
            if (is_null(val, t_farm)) {
                t_syntaxer.set_upcase_mode(false);
            }
        }
    }
}

Cell CoreEnvironment::eval_fnc(
    Cell& fnc,
    CarCdrIterator args_beg_it,
    CarCdrIterator args_end_it,
    bool forse_nospread_args
)
{                                                                               
    if (is_alambda_form(fnc, t_farm)) {
        auto l = get_lambda_form(fnc);
        t_l_evaler.push({ l, args_beg_it,args_end_it,forse_nospread_args });
        return t_l_evaler.pop_eval();
    }   

    if (auto fnc_r = t_funcs.find(to_symbol(fnc)); !holds_alternative<monostate>(fnc_r)) {


        if (holds_alternative<FuncsStorage::bifunc>(fnc_r) || holds_alternative<FuncsStorage::nbifunc>(fnc_r)) {
            bifunc ptr = nullptr;
            if (holds_alternative<FuncsStorage::bifunc>(fnc_r)) {
                ptr = get<FuncsStorage::bifunc>(fnc_r).ptr;
            }
            else {
                ptr = get<FuncsStorage::nbifunc>(fnc_r).ptr;
            }

            t_bi_evaler.push({
                (holds_alternative<FuncsStorage::bifunc>(fnc_r)) ? bifunc_type::bifunc: bifunc_type::nbifunc,
                ptr,
                args_beg_it,
                args_end_it,
                forse_nospread_args,
                t_farm.make_empty_list_cell()
                });
            return t_bi_evaler.pop_eval();
        }
        else {
            t_l_evaler.push({get<std::reference_wrapper<lambda>>(fnc_r).get(),args_beg_it,args_end_it,forse_nospread_args });
            return t_l_evaler.pop_eval();
        }
    }                                                                      
    else {                                                                      
        throw "eval_fnc error " + to_string(
            t_farm.make_list_cell({ fnc, t_farm.make_list_cell(args_beg_it , args_end_it) }), t_farm
        );
    }                                                                           
}           

std::optional<Cell> CoreEnvironment::eval_implicit_cond(Cell& atom) {
    auto predicate_val = eval_quote(car(atom));
    if (!is_null(predicate_val, t_farm)) {
        auto& cdr_buf = cdr(atom);
        if (!is_list(cdr_buf) || is_null(cdr_buf, t_farm))
        {
            return predicate_val;
        }
        CarCdrIteration iteration(cdr_buf, t_farm);
        return eval_direct_bifunc(&CoreEnvironment::nbifunc_progn, begin(iteration), end(iteration));
    }
    return nullopt;
}

Cell CoreEnvironment::eval_atom(const Cell& atom) {
    if (is_symbol(atom)) {
        return eval_symbol(atom);
    }
    return atom;
}

Cell CoreEnvironment::eval_symbol(const Cell& symbol) {
    if (is_null(symbol, t_farm)) {
        return t_farm.nil;
    }
    if (auto val = t_envs.get(to_symbol(symbol))) {
        return *val;
    }
    return symbol;
}


CoreEnvironment::CoreEnvironment():
    t_farm(this),
    t_funcs(t_farm),
    t_l_evaler(this),
    t_bi_evaler(this),
    t_streams(nullopt),
    t_syntaxer(t_farm),
    t_direct_call_buf(t_farm.nil),
    t_findbifunc_buf(false, { bifunc_type::bifunc, nullptr })
{
}

CoreEnvironment::CoreEnvironment(CoreEnvStreamsProvider& streams) :
    CoreEnvironment()
{
    t_streams = streams;
}


std::vector<std::string> CoreEnvironment::execute_all(CoreInputStreamInt& stream, const IMutexed<bool>& stop_flag)
{
    t_stop_flag = { stop_flag };
    #ifndef EX_CATCH
        std::vector<std::string> result;
        auto s = t_syntaxer.read_sexprs_stream(stream);
        while (s.ready())
        {
            auto [reason, c] = s.read();
            if (reason == read_reason::empty_input) {
                break;
            }
            if (reason != read_reason::success) {
                throw "input error";
            }
            result.emplace_back(to_string(eval_quote(c), t_farm));
        }
        return result;
    #endif

    #ifdef EX_CATCH
        try
        {
            std::vector<std::string> result;
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
                result.emplace_back(to_string(eval_quote(c), t_farm));
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
    #endif
}

std::string CoreEnvironment::execute_one(CoreInputStreamInt& stream, const IMutexed<bool>& stop_flag)
{
    t_stop_flag = { stop_flag };
    auto [read_reason, c] = t_syntaxer.read_sexpr(stream);
    if (read_reason != read_reason::success) throw "input error";

    #ifndef EX_CATCH
        return to_string(eval_quote(c), t_farm);
    #endif

    #ifdef EX_CATCH
        try
        {
            return to_string(eval_quote(c), t_farm);
        }
        catch (...)
        {
            t_envs.clear_subenvs();
            t_args.clear();
            t_l_evaler.clear();
            t_bi_evaler.clear();
            throw;
        }
    #endif
}

const CellEnvironment::mp& CoreEnvironment::vars() const
{
    return t_envs.get_globals();
}

void CoreEnvironment::set_streams(CoreEnvStreamsProvider& streams)
{
    t_streams = streams;
}

Cell CoreEnvironment::bifunc_atomp() {
    if (t_args == 0) return t_farm.T;
    if (is_list(arg1) && is_null(to_list(arg1))) return t_farm.T;
    return bool_cast(is_atom(arg1), t_farm);
}

Cell CoreEnvironment::bifunc_symbolp() {
    if (t_args == 0) return t_farm.T;
    if (is_list(arg1) && is_null(to_list(arg1))) return t_farm.T;
    return bool_cast(is_symbol(arg1), t_farm);
}

Cell CoreEnvironment::bifunc_numberp() {
    if (t_args == 0) return t_farm.nil;
    return bool_cast(is_number((arg1)), t_farm);
}

Cell CoreEnvironment::bifunc_listp() {
    if (t_args == 0) return t_farm.T;
    return bool_cast(is_list(arg1), t_farm);
}

Cell CoreEnvironment::bifunc_null() {
    if (t_args == 0) return t_farm.T;
    return bool_cast(is_null((arg1), t_farm), t_farm);
}

Cell CoreEnvironment::bifunc_add()
{
    if (t_args == 0) return t_farm.make_number_cell(BigInt(0l));
    if (!is_number(arg1)) throw "bifunc_add error";
    Number n = to_number(arg1);

    for (auto it = next(begin(t_args) , 1); it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_add error";
        n += to_number(*it);
    }
    return t_farm.make_number_cell(move(n));
}

Cell CoreEnvironment::bifunc_sub()
{
    if (t_args == 0) throw "bifunc_sub error";
    if (t_args == 1) {
        if (!is_number(arg1)) throw "bifunc_sub error";
        Number n = to_number(arg1);
        n.minus();
        return  t_farm.make_number_cell(move(n));
    }
    if (!is_number(arg1)) throw "bifunc_sub error";
    Number n = to_number(arg1);

    for (auto it = next(begin(t_args) , 1); it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_sub error";
        n -= to_number(*it);
    }
    return t_farm.make_number_cell(move(n));
}

Cell CoreEnvironment::bifunc_mul()
{
    if (t_args == 0) {
        return t_farm.make_number_cell(BigInt(1l));
    }
    if (!is_number(arg1)) throw "bifunc_mul error";
    if (t_args == 1) {
        return arg1;
    }
    Number n = to_number(arg1);

    for (auto it = next(begin(t_args) , 1); it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_mul error";
        n *= to_number(*it);
    }
    return t_farm.make_number_cell(move(n));
}

Cell CoreEnvironment::bifunc_div()
{
    if (t_args == 0) throw "bifunc_div error";
    if (t_args == 1) {
        if (!is_number(arg1)) throw "bifunc_div error";
        Number n = t_farm.make_number(1);
        n /= to_number(arg1);
        return t_farm.make_number_cell(move(n));
    }

    if (!is_number(arg1)) throw "bifunc_div error";
    Number n = to_number(arg1);

    if (!is_number(arg2)) throw "bifunc_div error";
    Number n2 = to_number(arg2);

    for (auto it = next(begin(t_args) , 2); it != end(t_args); ++it) {
        if (!is_number(*it)) throw "bifunc_div error";
        n2 *= to_number(*it);
    }

    if (n2 == t_farm.make_number(BigInt(0l))) {
        throw "bifunc_div error (Zero Divide)";
    }

    n /= n2;

    return t_farm.make_number_cell(move(n));
}

Cell CoreEnvironment::bifunc_greater()
{
    return numbers_compare(numbers_compare_type::greater);
}

Cell CoreEnvironment::bifunc_greater_equal()
{
    return numbers_compare(numbers_compare_type::greater_eq);
}

Cell CoreEnvironment::bifunc_less()
{
    return numbers_compare(numbers_compare_type::less);
}

Cell CoreEnvironment::bifunc_less_equal()
{
    return numbers_compare(numbers_compare_type::less_eq);
}

Cell CoreEnvironment::bifunc_num_eq()
{
    return numbers_compare(numbers_compare_type::eq);
}

Cell CoreEnvironment::bifunc_equal()
{
    if (t_args == 0) return t_farm.T;
    if (t_args == 1) return t_farm.T;
    if (is_number(arg1) && is_number(arg2)) {
        return eval_direct_bifunc(&CoreEnvironment::bifunc_num_eq, begin(t_args), end(t_args));
    }

    if (is_symbol(arg1) && is_symbol(arg2)) {
        return eval_direct_bifunc(&CoreEnvironment::bifunc_eq, begin(t_args), end(t_args));
    }

    if (is_list(arg1) && is_list(arg2)) {
        reference_wrapper<const Cell> s1 = arg1;
        reference_wrapper<const Cell> s2 = arg2;
        
        while (!is_null(s1, t_farm) && !is_null(s2, t_farm))
        {
            auto lst = t_farm.make_list_cell({ car(s1), car(s2) });
            CarCdrIteration iteration(lst, t_farm);
            if (
                is_null(eval_direct_bifunc(&CoreEnvironment::bifunc_equal, begin(iteration), end(iteration)), t_farm)
                )
            {
                return t_farm.nil;
            }
            s1 = cdr(s1);
            s2 = cdr(s2);
        } 
        if (is_null(s1, t_farm) && is_null(s2, t_farm)) return t_farm.T;
        auto lst = t_farm.make_list_cell ({ car(s1), car(s2) });
        CarCdrIteration iteration(lst, t_farm);
        return bool_cast(
            !is_null(eval_direct_bifunc(&CoreEnvironment::bifunc_equal, begin(iteration), end(iteration)), t_farm), t_farm
        );
    }

    return t_farm.nil;
}

Cell CoreEnvironment::bifunc_car()
{
    Cell s = (t_args >= 1) ? arg1 : t_farm.nil;

    if (is_atom(s)) {
        return eval_atom(s);
    }
    else {
        if (is_null(s, t_farm)) {
            return t_farm.nil;
        }
        else {
            return car(s);
        }
    }
}

Cell CoreEnvironment::bifunc_cdr()
{
    Cell s = (t_args >= 1) ? arg1 : t_farm.nil;
    if (is_null(s, t_farm)) {
        return t_farm.nil;
    }
    else if (is_atom(s)) {
        throw "bifunc_cdr error (property not available)";
    }
    else {
        return cdr(s);
    }
}

Cell CoreEnvironment::bifunc_append()
{
    if (t_args == 0) return t_farm.nil;
    auto it = begin(t_args);
    while ((is_atom(*it) || is_null(to_list(*it))) && it != end(t_args)) {
        ++it;
    }
    if(it == end(t_args))return t_farm.nil;
    if(next(it, 1) == end(t_args))return *it;
    Cell s = *it;
    ++it;
    for(; it != end(t_args); ++it){
        s = append(s, *it, t_farm);
    }
    return s;
}

Cell CoreEnvironment::bifunc_cons()
{
    Cell s1 = (t_args >= 1) ? arg1: t_farm.nil;
    Cell s2 = (t_args >= 2) ? arg2 : t_farm.nil;

    return cons(s1, s2, t_farm);
}

Cell CoreEnvironment::bifunc_list()
{
    return t_farm.make_list_cell(begin(t_args), end(t_args));
}

Cell CoreEnvironment::bifunc_getd()
{
    if (t_args == 0) return t_farm.nil;
    if(!is_symbol(arg1)) throw "bifunc_getf error";
    
    auto func = t_funcs.find(to_symbol(arg1));

    if (auto fnc_r = t_funcs.find(to_symbol(arg1)); !holds_alternative<monostate>(fnc_r)) {


        if (holds_alternative<FuncsStorage::bifunc>(fnc_r) || holds_alternative<FuncsStorage::nbifunc>(fnc_r)) {
            return t_farm.nil;
        }
        else {
            return t_farm.make_symbol_cell(to_string(get<std::reference_wrapper<lambda>>(fnc_r).get(), t_farm));
        }
    }
    return t_farm.nil;
}

Cell CoreEnvironment::bifunc_read() {
    if (t_streams) {
        // stop input?
        auto [reason, cell] = t_syntaxer.read_sexpr((*t_streams).get().t_input_stream());
        if (reason == read_reason::success) { return cell; }
        else return t_farm.nil;
    }
    return t_farm.nil;
    //if (t_stop_flag && (*t_stop_flag).get().get()) throw throw_stop_helper{};
    //if (!t_streams.t_input_stream().alive()) return t_ferm.symbols->nil;
}

Cell CoreEnvironment::bifunc_prog1()
{
    if (t_args == 0) return t_farm.nil;
    Cell result = arg1;
    eval_direct_bifunc(&CoreEnvironment::nbifunc_progn, next(begin(t_args) , 1), end(t_args));
    return result;
}

Cell CoreEnvironment::nbifunc_loop()
{
    auto it = begin(t_args);
    for(;;) {
        if(it == end(t_args)) it = begin(t_args);
        if (is_implicit_cond(*it, t_farm)) {
            auto cellopt = eval_implicit_cond(*it);
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
    if (t_streams) {
        if (t_args == 0) {
            (*t_streams).get().t_output_stream().out_new_line(nil_str);
            return t_farm.nil;
        }
        else {
            (*t_streams).get().t_output_stream().out_new_line(to_string(arg1, t_farm));
            return arg1;
        }
    }
    return t_farm.nil;
}

Cell CoreEnvironment::nbifunc_quote()
{
    if (t_args == 0) return t_farm.nil;
    return arg1;
}

Cell CoreEnvironment::nbifunc_defun() {
    if (t_args < 2)  return t_farm.nil;
    if (!is_symbol(arg1)) return t_farm.nil;
    const auto& name = to_symbol(arg1);
    if (is_list(arg2)) {
        if (is_alambda_form(arg2, t_farm))
        {
            t_funcs.add_lambda(name, get_lambda_form(arg2));
        }
        else {
            auto body = t_farm.make_list_cell(
                next(begin(t_args), 2),
                end(t_args)
            );
            t_funcs.add_lambda(
                name,
                make_spread_lambda_form(
                    lambda_types::lambda,
                    arg2,
                    body
                )
            );
        }
    }
    else if(is_symbol(arg2)) {
        auto body = t_farm.make_list_cell(
            next(begin(t_args), 2),
            end(t_args)
        );
        t_funcs.add_lambda(
            name,
            make_nospread_lambda_form(
                lambda_types::lambda,
                to_symbol(arg2),
                body
            )
        );
    }
    else {
        throw "nbifunc_defun error";
    }
    return arg1;
}

Cell CoreEnvironment::eval_quote(Cell& arg) {
    if ((t_stop_flag) && (*t_stop_flag).get().get()) throw throw_stop_helper{};
    if (is_list(arg)) {
        if (is_null(to_list(arg))) return t_farm.nil;
        if (auto& cdr_buf = cdr(arg); is_list(cdr_buf)) {
            CarCdrIteration iteration(cdr_buf, t_farm);
            return eval_fnc(car(arg), begin(iteration), end(iteration), false);
        }
        else {
            auto lst_buf = t_farm.make_list_cell({ cdr_buf });
            CarCdrIteration iteration(lst_buf, t_farm);
            return eval_fnc(car(arg), begin(iteration), end(iteration), false);
        }
    }
    else {
        return eval_atom(arg);
    }
}

Cell CoreEnvironment::nbifunc_cond()
{
    for (auto it = begin(t_args); it != end(t_args); ++it) {
        auto& elem = *it;
        if (!is_list(elem)) {
            if(is_null_symbol(elem, t_farm)) continue;
            else throw "bifunc_cond error";
        }

        auto predicate_val = eval_quote(car(elem));
        if (!is_null(predicate_val, t_farm)) {
            auto& cdr_buf = cdr(elem);
            if (!is_list(cdr_buf) || is_null(to_list(cdr_buf)))
            {
                return predicate_val;
            }
            CarCdrIteration iteration(cdr_buf, t_farm);
            return eval_direct_bifunc(&CoreEnvironment::nbifunc_progn, begin(iteration), end(iteration));
        }
    }
    return t_farm.nil;
}

Cell CoreEnvironment::nbifunc_progn()
{
    if (t_args == 0) return t_farm.nil;
    auto it = begin(t_args);
    for (; next(it) != end(t_args); ++it) {
        if (is_implicit_cond(*it, t_farm)) {
            auto cellopt = eval_implicit_cond(*it);
            if (cellopt) return *cellopt;
        }
        else {
            eval_quote(*it);
        }
    }

    auto& last = *it;
    if (is_implicit_cond(last, t_farm)) {
        auto cellopt = eval_implicit_cond(last);
        if (cellopt) return *cellopt;
        else throw "nbifunc_progn error -> implicit cond at the end, but t_ferm.symbols->nil";
    }
    else {
        return eval_quote(last);
    }
}

Cell CoreEnvironment::bifunc_set()
{
    if (t_args == 0) return t_farm.nil;
    auto& val = (next(begin(t_args)) == end(t_args)) ? t_farm.nil : arg2;
    set_value(arg1, val);
    return val;
}

Cell CoreEnvironment::bifunc_eq()
{
    if (t_args == 0) return t_farm.nil;
    if (t_args == 1 && is_list(arg1) && is_null(to_list(arg1))) return t_farm.T;
    if (t_args == 1) return t_farm.nil;
    const auto& s1 = arg1;
    const auto& s2 = arg2;

    if (!is_atom(s1) || !is_atom(s2)) {
        return t_farm.nil;
    }
    else if (is_number(s1) && is_number(s2)) {
        return bool_cast(to_number(s1) == to_number(s2), t_farm);
    }
    else if (is_symbol(s1) && is_symbol(s2)) {
        return bool_cast(to_symbol(s1) == to_symbol(s2), t_farm);
    }
    return t_farm.nil;
}

Cell CoreEnvironment::bifunc_last()
{
    if (t_args == 0) return t_farm.nil;
    if (is_atom(arg1)) {
        return t_farm.nil;
    }
    else {
        if (is_null(arg1, t_farm)) {
            return t_farm.nil;
        }
        CarCdrIteration iteration(arg1, t_farm);
        auto it = begin(iteration);
        while (next(it) != end(iteration)) {
            ++it;
        }
        return it.get_elem();
    } 
}

Cell CoreEnvironment::bifunc_length()
{
    if (t_args == 0) return t_farm.make_number_cell(BigInt(0l));
    auto& s = arg1;
    if (is_atom(s)) {
        throw "bifunc_length error (wtf ???)";
    }
    else {
        CarCdrIteration iteration(s, t_farm);
        return t_farm.make_number_cell(BigInt(static_cast<long>(std::distance(begin(iteration), end(iteration)))));
    }
 }

Cell CoreEnvironment::bifunc_apply()
{
    if (t_args == 0) throw "bifunc_apply error (unknown function)";
    if (t_args >= 2) {
        if(is_list(arg2)) {
            CarCdrIteration iteration(arg2, t_farm);
            return eval_fnc(arg1, begin(iteration), end(iteration), true );
        }
        auto eval_buf = t_farm.make_list_cell({ arg1 });
        return eval_quote(eval_buf);
    }
    else {
        auto eval_buf = t_farm.make_list_cell({ arg1 });
        return eval_quote(eval_buf);
    }
}

Cell CoreEnvironment::nbifunc_setq()
{
    if (t_args == 0) return t_farm.nil;
    auto it = next(begin(t_args) , 1);
    auto val = (it == end(t_args)) ? t_farm.nil : eval_quote(*it);

    set_value(arg1, val);

    if (it != end(t_args) && next(it, 1) != end(t_args)) {
        return eval_direct_bifunc(&CoreEnvironment::nbifunc_setq, next(it, 1), end(t_args));
    }
    else {
        return val;
    }
}

Cell CoreEnvironment::bifunc_eval() {
    if (t_args == 0) return t_farm.nil;
    return eval_quote(arg1);
}

Cell CoreEnvironment::bifunc_integerp()
{
    if (t_args == 0) return t_farm.nil;
    return bool_cast(is_integer(arg1), t_farm);
}

Cell CoreEnvironment::bifunc_oblist()
{
    const auto& lst = t_farm.symbols->get_lst();
    std::list<Cell> a;
    std::vector <Cell> result;
    for (const auto& symb : lst) {
        result.push_back(t_farm.make_symbol_cell(symb.data()));
    }
    return t_farm.make_list_cell(begin(result), end(result));
}

Cell CoreEnvironment::bifunc_rplaca()
{
    Cell s1 = (t_args >= 1) ? arg1 : t_farm.nil;
    Cell s2 = (t_args >= 2) ? arg2 : t_farm.nil;

    if (!is_symbol(s1) && !is_null(to_list(s1))) {
        rplaca(to_list(s1), s2);
        return s1;
    }
    return t_farm.nil;
}

Cell CoreEnvironment::bifunc_rplacd()
{
    Cell s1 = (t_args >= 1) ? arg1 : t_farm.nil;
    Cell s2 = (t_args >= 2) ? arg2 : t_farm.nil;

    if (!is_symbol(s1) && !is_null(to_list(s1))) {
        rplacd(to_list(s1), s2);
        return s1;
    }
    return t_farm.nil;
}

Cell CoreEnvironment::bifunc_copy_tree()
{
    Cell s1 = (t_args >= 1) ? arg1 : t_farm.nil;
    if (!is_list(s1) || is_null_symbol(s1, t_farm)) {
        return s1;
    }
    return tree_copy(s1, t_farm);
}
