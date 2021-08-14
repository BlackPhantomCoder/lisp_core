#include "CoreEnv.h"
#include "Funcs.h"
#include "ArgsCounter.h"

#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;
using namespace CoreData;

#define arg1 *b
#define arg2 *(next(b, 1))
#define arg3 *(next(b, 2))

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
    if (is_null(cdr(lst))) throw "get_lambda_form error";
    auto& second = car(cdr(lst));
    if (is_list(second)) {
        return make_spread_lambda_form(
            (to_symbol(car(lst)) == to_symbol(t_farm.lambda_symbol)) ? lambda_types::lambda : lambda_types::nlambda,
            second,
            cdr(cdr(lst))
        );
    }
    else if(is_symbol(second)){
        return make_nospread_lambda_form(
            (to_symbol(car(lst)) == to_symbol(t_farm.lambda_symbol)) ? lambda_types::lambda : lambda_types::nlambda,
            to_symbol(second),
            cdr(cdr(lst))
        );
    }
    throw "get_lambda_form error";
}

Cell CoreEnvironment::numbers_compare(iter b, iter e, numbers_compare_type type)
{
    if (ArgsCounter{b, e} == 0) return t_farm.T;
    if (ArgsCounter{b, e} == 1) return t_farm.T;
    if (!is_number(arg1)) throw "numbers_compare: error";
    const auto* n = &to_number(arg1);

    bool result = true;
    auto it = next(b);
    for (; it != e; ++it) {
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
        if (!is_null(name)) {
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

        //special vars
        if (symb == to_symbol(t_farm.read_up_case_symbol)) {
            bool val_b = !is_null(val);
            t_syntaxer.set_upcase_mode(val_b);
            t_output_control.set_read_upcase(val_b);
        }
    }
}

Cell CoreEnvironment::eval_atom(const Cell& atom) {
    if (is_symbol(atom)) {
        return eval_symbol(atom);
    }
    return atom;
}

Cell CoreEnvironment::eval_symbol(const Cell& symbol) {
    if (is_null(symbol)) {
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
    t_streams(nullopt),
    t_syntaxer(t_farm),
    t_output_control(t_farm),
    t_funcs_evaler(this)
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
        result.emplace_back(t_output_control.to_string(t_execute(c)));
    }
    return result;
}

std::string CoreEnvironment::execute_one(CoreInputStreamInt& stream, const IMutexed<bool>& stop_flag)
{
    t_stop_flag = { stop_flag };
    auto [read_reason, c] = t_syntaxer.read_sexpr(stream);
    if (read_reason != read_reason::success) throw "input error";

    return t_output_control.to_string(t_execute(c));
}

const CellEnvironment::mp& CoreEnvironment::vars() const
{
    return t_envs.get_globals();
}

void CoreEnvironment::set_streams(CoreEnvStreamsProvider& streams)
{
    t_streams = streams;
}

Cell CoreEnvironment::t_execute(Cell& arg)
{
    #ifdef EX_CATCH
    try
    {
        auto result = t_funcs_evaler.eval(arg);

        //release memory
        t_clear_mem();

        return result;
    }
    catch (...)
    {
        t_envs.clear_subenvs();
        t_funcs_evaler.clear();
       
        t_clear_mem();

        throw;
    }
    #endif

    auto result = t_funcs_evaler.eval(arg);

    t_clear_mem();

    return result;
}

void CoreEnvironment::t_clear_mem()
{
    CoreData::allocator_release_memory<std::pair<Symbol, Cell>>();
    CoreData::allocator_release_memory<Cell>();
    CoreData::allocator_release_memory<const Cell*>();
}

Cell CoreEnvironment::bifunc_atomp(iter b, iter e) {
    if (ArgsCounter{b, e} == 0) return t_farm.T;
    if (is_list(arg1) && is_null_list(to_list(arg1))) return t_farm.T;
    return bool_cast(is_atom(arg1), t_farm);
}

Cell CoreEnvironment::bifunc_symbolp(iter b, iter e) {
    if (ArgsCounter{b, e} == 0) return t_farm.T;
    if (is_list(arg1) && is_null_list(to_list(arg1))) return t_farm.T;
    return bool_cast(is_symbol(arg1), t_farm);
}

Cell CoreEnvironment::bifunc_numberp(iter b, iter e) {
    if (ArgsCounter{b, e}== 0) return t_farm.nil;
    return bool_cast(is_number((arg1)), t_farm);
}

Cell CoreEnvironment::bifunc_listp(iter b, iter e) {
    if (ArgsCounter{b, e}== 0) return t_farm.T;
    return bool_cast(is_list(arg1), t_farm);
}

Cell CoreEnvironment::bifunc_null(iter b, iter e) {
    if (ArgsCounter{b, e}== 0) return t_farm.T;
    return bool_cast(is_null((arg1)), t_farm);
}

Cell CoreEnvironment::bifunc_add(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) return t_farm.make_number_cell(BigInt(0l));
    if (!is_number(arg1)) throw "bifunc_add error";
    Number n = to_number(arg1);

    for (auto it = next(b); it != e; ++it) {
        if (!is_number(*it)) throw "bifunc_add error";
        n += to_number(*it);
    }
    return t_farm.make_number_cell(move(n));
}

Cell CoreEnvironment::bifunc_sub(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) throw "bifunc_sub error";
    if (ArgsCounter{b, e}== 1) {
        if (!is_number(arg1)) throw "bifunc_sub error";
        Number n = to_number(arg1);
        n.minus();
        return  t_farm.make_number_cell(move(n));
    }
    if (!is_number(arg1)) throw "bifunc_sub error";
    Number n = to_number(arg1);

    for (auto it = next(b , 1); it != e; ++it) {
        if (!is_number(*it)) throw "bifunc_sub error";
        n -= to_number(*it);
    }
    return t_farm.make_number_cell(move(n));
}

Cell CoreEnvironment::bifunc_mul(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) {
        return t_farm.make_number_cell(BigInt(1l));
    }
    if (!is_number(arg1)) throw "bifunc_mul error";
    if (ArgsCounter{b, e}== 1) {
        return arg1;
    }
    Number n = to_number(arg1);

    for (auto it = next(b , 1); it != e; ++it) {
        if (!is_number(*it)) throw "bifunc_mul error";
        n *= to_number(*it);
    }
    return t_farm.make_number_cell(move(n));
}

Cell CoreEnvironment::bifunc_div(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) throw "bifunc_div error";
    if (ArgsCounter{b, e}== 1) {
        if (!is_number(arg1)) throw "bifunc_div error";
        Number n = t_farm.make_number(1);
        n /= to_number(arg1);
        return t_farm.make_number_cell(move(n));
    }

    if (!is_number(arg1)) throw "bifunc_div error";
    Number n = to_number(arg1);

    if (!is_number(arg2)) throw "bifunc_div error";
    Number n2 = to_number(arg2);

    for (auto it = next(b , 2); it != e; ++it) {
        if (!is_number(*it)) throw "bifunc_div error";
        n2 *= to_number(*it);
    }

    if (n2 == t_farm.make_number(BigInt(0l))) {
        throw "bifunc_div error (Zero Divide)";
    }

    n /= n2;

    return t_farm.make_number_cell(move(n));
}

Cell CoreEnvironment::bifunc_greater(iter b, iter e)
{
    return numbers_compare(b, e, numbers_compare_type::greater);
}

Cell CoreEnvironment::bifunc_greater_equal(iter b, iter e)
{
    return numbers_compare(b, e, numbers_compare_type::greater_eq);
}

Cell CoreEnvironment::bifunc_less(iter b, iter e)
{
    return numbers_compare(b, e, numbers_compare_type::less);
}

Cell CoreEnvironment::bifunc_less_equal(iter b, iter e)
{
    return numbers_compare(b, e, numbers_compare_type::less_eq);
}

Cell CoreEnvironment::bifunc_num_eq(iter b, iter e)
{
    return numbers_compare(b, e, numbers_compare_type::eq);
}

Cell CoreEnvironment::bifunc_equal(iter b, iter e)
{
    const auto& s1 = (ArgsCounter{b, e} > 0) ? arg1 : t_farm.nil;
    const auto& s2 = (ArgsCounter{b, e} > 1) ? arg2 : t_farm.nil;
    if (is_number(s1) && is_number(s2)) {
        return bifunc_num_eq(b, e);
    }

    if (is_symbol(s1) && is_symbol(s2)) {
        return bifunc_eq(b, e);
    }

    if (is_list(s1) && is_list(s2)) {
        reference_wrapper<const Cell> s1_b = s1;
        reference_wrapper<const Cell> s2_b = s2;
        
        while (!is_null(s1_b) && !is_null(s2_b))
        {
            auto lst = t_farm.make_list_cell({ car(s1_b), car(s2_b) });
            if (
                is_null(bifunc_equal(begin(lst), end(lst)))
                )
            {
                return t_farm.nil;
            }
            s1_b = cdr(s1_b);
            s2_b = cdr(s2_b);
        } 
        if (is_null(s1_b) && is_null(s2_b)) return t_farm.T;
        auto lst = t_farm.make_list_cell ({ car(s1_b), car(s2_b) });
        return bool_cast(
            !is_null(bifunc_equal(begin(lst), end(lst))), t_farm
        );
    }

    if (is_null(s1) && is_null(s2)) {
        return t_farm.T;
    }

    return t_farm.nil;
}

Cell CoreEnvironment::bifunc_car(iter b, iter e)
{
    Cell s = (ArgsCounter{b, e} >= 1) ? arg1 : t_farm.nil;

    if (is_atom(s)) {
        return eval_atom(s);
    }
    else {
        if (is_null_list(to_list(s))) {
            return t_farm.nil;
        }
        else {
            return car(s);
        }
    }
}

Cell CoreEnvironment::bifunc_cdr(iter b, iter e)
{
    Cell s = (ArgsCounter{b, e} >= 1) ? arg1 : t_farm.nil;
    if (is_null(s)) {
        return t_farm.nil;
    }
    else if (is_atom(s)) {
        throw "bifunc_cdr error (property not available)";
    }
    else {
        return cdr(s);
    }
}

Cell CoreEnvironment::bifunc_cons(iter b, iter e)
{
    Cell s1 = (ArgsCounter{b, e} >= 1) ? arg1: t_farm.nil;
    Cell s2 = (ArgsCounter{b, e} >= 2) ? arg2 : t_farm.nil;

    return cons(s1, s2, t_farm);
}

Cell CoreEnvironment::bifunc_list(iter b, iter e)
{
    return t_farm.make_list_cell(b, e);
}

Cell CoreEnvironment::bifunc_getd(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) return t_farm.nil;
    if(!is_symbol(arg1)) throw "bifunc_getf error";
    
    auto func = t_funcs.find(to_symbol(arg1));

    if (auto fnc_r = t_funcs.find(to_symbol(arg1)); !holds_alternative<monostate>(fnc_r)) {


        if (holds_alternative<FuncsStorage::bifunc>(fnc_r) || holds_alternative<FuncsStorage::nbifunc>(fnc_r)) {
            return t_farm.nil;
        }
        else {
            return gen_cell(get<std::reference_wrapper<lambda>>(fnc_r).get(), t_farm);
        }
    }
    return t_farm.nil;
}

Cell CoreEnvironment::bifunc_read(iter b, iter e) {
    if (t_streams) {
        // stop input?
        auto [reason, cell] = t_syntaxer.read_sexpr((*t_streams).get().t_input_stream());
        if (reason == read_reason::success) { return cell; }
        else return t_farm.nil;
    }
    return t_farm.nil;
    //if (t_stop_flag && (*t_stop_flag).get(iter b, iter e).get(iter b, iter e)) throw throw_stop_helper{};
    //if (!t_streams.t_input_stream(iter b, iter e).alive(iter b, iter e)) return t_ferm.symbols->nil;
}

Cell CoreEnvironment::bifunc_print(iter b, iter e)
{
    if (t_streams) {
        if (ArgsCounter{b, e}== 0) {
            (*t_streams).get().t_output_stream().out_new_line(nil_str);
            return t_farm.nil;
        }
        else {
            (*t_streams).get().t_output_stream().out_new_line(t_output_control.to_string(arg1));
            return arg1;
        }
    }
    return t_farm.nil;
}

Cell CoreEnvironment::nbifunc_quote(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) return t_farm.nil;
    return arg1;
}

Cell CoreEnvironment::nbifunc_defun(iter b, iter e) {
    if (ArgsCounter{b, e} < 2)  return t_farm.nil;
    if (!is_symbol(arg1)) return t_farm.nil;
    const auto& name = to_symbol(arg1);
    if (is_list(arg2)) {
        if (is_alambda_form(arg2, t_farm))
        {
            t_funcs.add_lambda(name, get_lambda_form(arg2));
        }
        else {
            auto body = t_farm.make_list_cell(
                next(b, 2),
                e
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
            next(b, 2),
            e
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

Cell CoreEnvironment::bifunc_set(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) return t_farm.nil;
    auto& val = (next(b) == e) ? t_farm.nil : arg2;
    set_value(arg1, val);
    return val;
}

Cell CoreEnvironment::bifunc_eq(iter b, iter e)
{
    const auto& s1 = (ArgsCounter{b, e} > 0) ? arg1 : t_farm.nil;
    const auto& s2 = (ArgsCounter{b, e} > 1) ? arg2 : t_farm.nil;


    if (is_number(s1) && is_number(s2)) {
        return bool_cast(to_number(s1) == to_number(s2), t_farm);
    }
    else if (is_symbol(s1) && is_symbol(s2)) {
        return bool_cast(to_symbol(s1) == to_symbol(s2), t_farm);
    }
    else if(is_null(s1) && is_null(s2)) {
        return t_farm.T;
    }
    return t_farm.nil;
}

Cell CoreEnvironment::bifunc_last(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) return t_farm.nil;
    if (is_atom(arg1)) {
        return t_farm.nil;
    }
    else {
        if (is_null_list(to_list(arg1))) {
            return t_farm.nil;
        }
        auto it = begin(arg1);
        while (next(it) != end(arg1)) {
            ++it;
        }
        return it.get_elem();
    } 
}

Cell CoreEnvironment::bifunc_length(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) return t_farm.make_number_cell(BigInt(0l));
    auto& s = arg1;
    if (is_atom(s)) {
        throw "bifunc_length error (wtf ???)";
    }
    else {
        return t_farm.make_number_cell(BigInt(static_cast<long>(std::distance(begin(s), end(s)))));
    }
 }

Cell CoreEnvironment::bifunc_integerp(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) return t_farm.nil;
    return bool_cast(is_integer(arg1), t_farm);
}

Cell CoreEnvironment::bifunc_oblist(iter b, iter e)
{
    const auto& lst = t_farm.symbols->get_lst();
    std::vector <Cell> result;
    result.reserve(lst.size());
    for (const auto& symb : lst) {
        result.push_back(t_farm.make_symbol_cell(symb.data()));
    }
    return t_farm.make_list_cell(begin(result), end(result));
}

Cell CoreEnvironment::bifunc_rplaca(iter b, iter e)
{
    Cell s1 = (ArgsCounter{b, e} >= 1) ? arg1 : t_farm.nil;
    Cell s2 = (ArgsCounter{b, e} >= 2) ? arg2 : t_farm.nil;

    if (!is_symbol(s1) && !is_null_list(to_list(s1))) {
        rplaca(to_list(s1), s2);
        return s1;
    }
    return t_farm.nil;
}

Cell CoreEnvironment::bifunc_rplacd(iter b, iter e)
{
    Cell s1 = (ArgsCounter{b, e} >= 1) ? arg1 : t_farm.nil;
    Cell s2 = (ArgsCounter{b, e} >= 2) ? arg2 : t_farm.nil;

    if (!is_symbol(s1) && !is_null_list(to_list(s1))) {
        rplacd(to_list(s1), s2);
        return s1;
    }
    return t_farm.nil;
}

Cell CoreEnvironment::bifunc_copy_tree(iter b, iter e)
{
    Cell s1 = (ArgsCounter{b, e} >= 1) ? arg1 : t_farm.nil;
    if (!is_list(s1) || is_null_symbol(s1)) {
        return s1;
    }
    return tree_copy(s1, t_farm);
}

Cell CoreEnvironment::bifunc_pack(iter b, iter e)
{
    if (b == e || !is_list(arg1) || is_null(arg1)) return t_farm.make_symbol_cell("");
    std::string s;
    auto it = begin(*b);
    auto e_it = end(*b);
    for (; it != e_it; ++it) {
        if (is_atom(*it)) {
            s += t_output_control.to_string_raw(*it);
        }
    }
    return t_farm.make_symbol_cell(move(s));
}

Cell CoreEnvironment::bifunc_unpack(iter b, iter e)
{
    if (b == e || is_list(arg1) || is_null_symbol(arg1)) return t_farm.nil;
    string s = t_output_control.to_string_raw(arg1);
    vector<Cell> buf;
    buf.reserve(s.size());
    for (char c : s) {
        buf.push_back(t_farm.make_symbol_cell(string() + c));
    }
    return t_farm.make_list_cell(begin(buf), end(buf));
}
