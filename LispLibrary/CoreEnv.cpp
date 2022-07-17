#include "CoreEnv.h"
#include "Funcs.h"
#include "ArgsCounter.h"
#include "profile.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include "SupportFuncs.h"
#include "Bifuncs.h"
#include "json/include/json.hpp"

using namespace std;
using namespace CoreData;
using namespace nlohmann;

#define arg1 *b
#define arg2 *(next(b, 1))
#define arg3 *(next(b, 2))






lambda support_funcs::make_spread_lambda_form(
    lambda_types lambda_type,
    const Cell& params,
    Cell& body
)
{
    return  make_lambda(lambda_type, lambda_args_types::spread, params, body, env->farm());
}

lambda support_funcs::make_nospread_lambda_form(
    lambda_types lambda_type,
    const Symbol& param,
    Cell& body
)
{
    return  make_lambda(lambda_type, lambda_args_types::nospread, env->farm().make_symbol_cell( param ), body, env->farm());
}

macro support_funcs::make_spread_macro_form(
    const Cell& params,
    const Cell& body
)
{
    auto buf = tree_copy(params, env->farm());
    return  make_macro(lambda_args_types::spread, buf, body, env->farm());
}

macro support_funcs::make_nospread_macro_form(
    const Symbol& param,
    const Cell& body
)
{
    auto buf = env->farm().make_symbol_cell(param);
    return  make_macro(lambda_args_types::nospread, buf, body, env->farm());
}

lambda support_funcs::make_lambda_form(Cell& lst)
{
    if (is_null(cdr(lst))) throw "get_lambda_form error";
    auto& second = car(cdr(lst));
    if (is_list(second)) {
        return make_spread_lambda_form(
            (to_symbol(car(lst)) == to_symbol(env->farm().lambda_symbol())) ? lambda_types::lambda : lambda_types::nlambda,
            second,
            cdr(cdr(lst))
        );
    }
    else if(is_symbol(second)){
        return make_nospread_lambda_form(
            (to_symbol(car(lst)) == to_symbol(env->farm().lambda_symbol())) ? lambda_types::lambda : lambda_types::nlambda,
            to_symbol(second),
            cdr(cdr(lst))
        );
    }
    throw "get_lambda_form error";
}

Cell CoreEnvironment::t_numbers_compare(iter b, iter e, numbers_compare_type type)
{
    if (ArgsCounter{b, e} == 0) return t_farm.T();
    if (ArgsCounter{b, e} == 1) return t_farm.T();
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

void CoreEnvironment::t_clear()
{
    t_envs.clear_subenvs();
    //t_funcs_evaler.clear();

    t_clear_mem();
}

SExprsFarm& CoreEnvironment::farm()
{
    return t_farm;
}

std::optional<std::reference_wrapper<const IMutexed<bool>>>& CoreEnvironment::stop_flag()
{
    return t_stop_flag;
}

std::optional<std::reference_wrapper<CoreOutputStreamInt>>& CoreEnvironment::cos()
{
    return t_cos;
}

support_funcs& CoreEnvironment::support_funcs()
{
    return t_support;
}

OutputController& CoreEnvironment::output_control()
{
    return t_output_control;
}

CellEnvironment& CoreEnvironment::envs()
{
    return t_envs;
}

MacroTable& CoreEnvironment::macrotable()
{
    return t_macrotable;
}

Input& CoreEnvironment::input()
{
    return t_input;
}

Scanner& CoreEnvironment::scanner()
{
    return t_scanner;
}

FuncsStorage& CoreEnvironment::funcs()
{
    return t_funcs;
}

CellSerializer& CoreEnvironment::cserial()
{
    return t_cserial;
}

void support_funcs::set_value(const Cell& name, const Cell& val)
{
    if (!is_symbol(name)) {
        if (!is_null(name)) {
            throw "bifunc_set error";
        }
    }
    else {
        const auto& symb = to_symbol(name);
        //cout << env->output_control().to_string(name) << " = " << env->output_control().to_string(val) << endl;
        env->envs().set(symb, val);

        //special vars
        if (symb == to_symbol(env->farm().read_up_case_symbol())) {
            bool val_b = !is_null(val);
            env->output_control().set_read_upcase(val_b);
        }
    }
}

Cell support_funcs::eval_atom(const Cell& atom) {
    if (is_symbol(atom)) {
        return eval_symbol(atom);
    }
    return atom;
}

Cell support_funcs::eval_symbol(const Cell& symbol) {
    if (is_null(symbol)) {
        return env->farm().nil();
    }
    if (auto val = env->envs().get(to_symbol(symbol)); !empty(val)) {
        return val;
    }
    return symbol;
}


CoreEnvironment::CoreEnvironment(std::optional<std::reference_wrapper<std::istream>> state):
    t_farm(*this),
    t_funcs(*this),
    t_input(*this),
    t_scanner(*this),
    t_envs(*this),
    t_support{ this },
    //t_funcs_evaler(this),
    t_macrotable(*this),
    t_cserial(*this),
    t_streams(nullopt),
    t_output_control(t_farm)
{
    if (state) {
        auto j  = json();
        *state >> j;
        t_farm.init(j.at(0));
        t_funcs.init(j.at(1));
        t_envs.init(j.at(2));
        t_macrotable.init(j.at(3));
    }
    else {
        t_farm.init(nullopt);
        t_funcs.init(nullopt);
        t_envs.init(nullopt);
        t_macrotable.init(nullopt);
    }
}

CoreEnvironment::CoreEnvironment(
    CoreEnvStreamsProviderInt& streams,
    std::optional<std::reference_wrapper<std::istream>> state
):
    CoreEnvironment(state)
{
    t_streams = streams;
}


void CoreEnvironment::save_state(std::ostream& os)
{
    auto j = json();
    t_farm.save_state(j.emplace_back());
    t_funcs.save_state(j.emplace_back());
    t_envs.save_state(j.emplace_back());
    t_macrotable.save_state(j.emplace_back());
    os << j;
}

bool CoreEnvironment::load_state(std::istream& is)
{
    
    auto j = json();
    try
    {
        is >> j;
    }
    catch (const std::exception& e)
    {
        cout << e.what() << endl;
        return false;
    }
    catch (...)
    {
        return false;
    }



    try
    {
        t_farm.load_state(j.at(0));
        t_funcs.load_state(j.at(1));
        t_envs.load_state(j.at(2));
        t_macrotable.load_state(j.at(3));
    }
    catch (const std::exception& e)
    {
        cout << e.what() << endl;
        throw;
    }
    catch (...)
    {
        throw;
        //return false;
    }
    return true;
}

void CoreEnvironment::execute_all(CoreEnvStreamsProviderInt& streams, const IMutexed<bool>& stop_flag)
{
    return t_core_env_under_catch(
        [this, &streams, &stop_flag]() -> void {
            t_prepare(streams, stop_flag);
            std::vector<std::string> result;
            while (t_input.alive()) {
                t_execute();
            }
        }
    );
}

void CoreEnvironment::execute_one(CoreEnvStreamsProviderInt& streams, const IMutexed<bool>& stop_flag)
{
    return t_core_env_under_catch(
        [this, &streams, &stop_flag]() -> void {
            t_prepare(streams, stop_flag);
            t_execute();
        }
    );
}

void CoreEnvironment::execute_driver(CoreEnvStreamsProviderInt& streams, const IMutexed<bool>& stop_flag)
{
    t_prepare(streams, stop_flag);
    while(t_input.alive())
    {
        t_core_env_under_catch(
            [this, &streams, &stop_flag]() -> void {
                t_cos->get().out_without_new_line("> ");
                auto c = t_execute(make_fnc<Read>(CarCdrIterator(), CarCdrIterator()));
                auto a = DurationFunc([this](std::chrono::milliseconds time) {
                    t_cos->get().out_new_line(": " + to_string(time.count()) + " ms");
                    });
                if (!(!t_input.alive() && is_symbol(c) && t_output_control.to_string_raw(c) == "")) {
                    t_cos->get().out_new_line(t_output_control.to_string(t_execute(c)));
                }
            }
        );
    }
}

void CoreEnvironment::set_streams(CoreEnvStreamsProviderInt& streams)
{
    t_streams = streams;
}

void CoreEnvironment::t_prepare(CoreEnvStreamsProviderInt& streams, const IMutexed<bool>& stop_flag)
{
    t_streams = streams;
    t_stop_flag = { stop_flag };
    t_input.change_cis(t_streams->get().t_input_stream());
    t_cos = t_streams->get().t_output_stream();
}

void CoreEnvironment::t_execute()
{
    auto c = t_execute(make_fnc<Read>(CarCdrIterator(), CarCdrIterator()));
    t_cos->get().out_new_line(t_output_control.to_string(t_execute(c)));
}

Cell CoreEnvironment::t_execute(Cell& arg)
{
    return t_execute(make_fnc<EvalQuote>(arg));
}

Cell CoreEnvironment::t_execute(CoreData::HolderPtr&& func)
{
    auto t_funcs_evaler = FuncsEvaler(this);
    return t_funcs_evaler.eval(move(func));
}

void CoreEnvironment::t_clear_mem()
{
    CoreData::allocator_release_memory<std::pair<Symbol, Cell>>();
    CoreData::allocator_release_memory<Cell>();
    CoreData::allocator_release_memory<const Cell*>();
    CoreData::funcs_pools_clear();
}

Cell CoreEnvironment::bifunc_atomp(iter b, iter e) {
    if (ArgsCounter{b, e} == 0) return t_farm.T();
    if (is_list(arg1) && is_null_list(to_list(arg1))) return t_farm.T();
    return bool_cast(is_atom(arg1), t_farm);
}

Cell CoreEnvironment::bifunc_symbolp(iter b, iter e) {
    if (ArgsCounter{b, e} == 0) return t_farm.T();
    if (is_list(arg1) && is_null_list(to_list(arg1))) return t_farm.T();
    return bool_cast(is_symbol(arg1), t_farm);
}

Cell CoreEnvironment::bifunc_numberp(iter b, iter e) {
    if (ArgsCounter{b, e}== 0) return t_farm.nil();
    return bool_cast(is_number((arg1)), t_farm);
}

Cell CoreEnvironment::bifunc_listp(iter b, iter e) {
    if (ArgsCounter{b, e}== 0) return t_farm.T();
    return bool_cast(is_list(arg1), t_farm);
}

Cell CoreEnvironment::bifunc_null(iter b, iter e) {
    if (ArgsCounter{b, e}== 0) return t_farm.T();
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
    return t_numbers_compare(b, e, numbers_compare_type::greater);
}

Cell CoreEnvironment::bifunc_greater_equal(iter b, iter e)
{
    return t_numbers_compare(b, e, numbers_compare_type::greater_eq);
}

Cell CoreEnvironment::bifunc_less(iter b, iter e)
{
    return t_numbers_compare(b, e, numbers_compare_type::less);
}

Cell CoreEnvironment::bifunc_less_equal(iter b, iter e)
{
    return t_numbers_compare(b, e, numbers_compare_type::less_eq);
}

Cell CoreEnvironment::bifunc_num_eq(iter b, iter e)
{
    return t_numbers_compare(b, e, numbers_compare_type::eq);
}

Cell CoreEnvironment::bifunc_equal(iter b, iter e)
{
    const auto& s1 = (ArgsCounter{b, e} > 0) ? arg1 : t_farm.nil();
    const auto& s2 = (ArgsCounter{b, e} > 1) ? arg2 : t_farm.nil();
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
                return t_farm.nil();
            }
            s1_b = cdr(s1_b);
            s2_b = cdr(s2_b);
        } 
        if (is_null(s1_b) && is_null(s2_b)) return t_farm.T();
        auto lst = t_farm.make_list_cell ({ car(s1_b), car(s2_b) });
        return bool_cast(
            !is_null(bifunc_equal(begin(lst), end(lst))), t_farm
        );
    }

    if (is_null(s1) && is_null(s2)) {
        return t_farm.T();
    }

    return t_farm.nil();
}

Cell CoreEnvironment::bifunc_car(iter b, iter e)
{
    Cell s = (ArgsCounter{b, e} >= 1) ? arg1 : t_farm.nil();

    if (is_atom(s)) {
        return t_support.eval_atom(s);
    }
    else {
        if (is_null_list(to_list(s))) {
            return t_farm.nil();
        }
        else {
            return car(s);
        }
    }
}

Cell CoreEnvironment::bifunc_cdr(iter b, iter e)
{
    Cell s = (ArgsCounter{b, e} >= 1) ? arg1 : t_farm.nil();
    if (is_null(s)) {
        return t_farm.nil();
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
    Cell s1 = (ArgsCounter{b, e} >= 1) ? arg1: t_farm.nil();
    Cell s2 = (ArgsCounter{b, e} >= 2) ? arg2 : t_farm.nil();

    return cons(s1, s2, t_farm);
}

Cell CoreEnvironment::bifunc_list(iter b, iter e)
{
    return t_farm.make_list_cell(b, e);
}

Cell CoreEnvironment::bifunc_getd(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) return t_farm.nil();
    if(!is_symbol(arg1)) throw "bifunc_getf error";
    
    auto func = t_funcs.find(to_symbol(arg1));

    if (auto fnc_r = t_funcs.find(to_symbol(arg1)); fnc_r) {
        auto& data = fnc_r->get();

        if (holds_alternative<FuncsStorage::bifunc>(data) || holds_alternative<FuncsStorage::nbifunc>(data)) {
            return t_farm.nil();
        }
        else if(holds_alternative<lambda>(data)){
            return gen_cell(get<lambda>(data), t_farm);
        }
        else if (holds_alternative<macro>(data)) {
            return gen_cell(get<macro>(data), t_farm);
        }
    }
    return t_farm.nil();
}

Cell CoreEnvironment::bifunc_print(iter b, iter e)
{
    if (t_streams) {
        if (ArgsCounter{b, e}== 0) {
            (*t_streams).get().t_output_stream().out_new_line(nil_str);
            return t_farm.nil();
        }
        else {
            (*t_streams).get().t_output_stream().out_new_line(t_output_control.to_string(arg1));
            return arg1;
        }
    }
    return t_farm.nil();
}

Cell CoreEnvironment::bifunc_prin1(iter b, iter e)
{
    if (t_streams) {
        if (ArgsCounter{ b, e } == 0) {
            (*t_streams).get().t_output_stream().out_without_new_line(nil_str);
            return t_farm.nil();
        }
        else {
            (*t_streams).get().t_output_stream().out_without_new_line(t_output_control.to_string(arg1));
            return arg1;
        }
    }
    return t_farm.nil();
}

Cell CoreEnvironment::nbifunc_quote(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) return t_farm.nil();
    return arg1;
}

Cell CoreEnvironment::nbifunc_defun(iter b, iter e) {
    if (ArgsCounter{b, e} < 2)  return t_farm.nil();
    if (!is_symbol(arg1)) return t_farm.nil();
    const auto& name = to_symbol(arg1);
    if (is_list(arg2)) {
        if (is_alambda_form(arg2, t_farm) || is_null_symbol(arg2))
        {
            t_funcs.set_func(name, t_support.make_lambda_form(arg2));
        }
        else {
            auto body = t_farm.make_list_cell(next(b, 2), e);
            t_funcs.set_func(
                name,
                t_support.make_spread_lambda_form(
                    lambda_types::lambda,
                    arg2,
                    body
                )
            );
        }
    }
    else if(is_symbol(arg2)) {
        auto body = t_farm.make_list_cell(next(b, 2), e);
        t_funcs.set_func(
            name,
            t_support.make_nospread_lambda_form(
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

Cell CoreEnvironment::nbifunc_defmacro(iter b, iter e)
{
    if (ArgsCounter{ b, e } < 2)  return t_farm.nil();
    if (!is_symbol(arg1)) return t_farm.nil();
    const auto& name = to_symbol(arg1);
    if (is_list(arg2) || is_null_symbol(arg2)) {
        auto body = t_farm.make_list_cell(next(b, 2), e);
        t_funcs.set_func(
            name,
            t_support.make_spread_macro_form(
                arg2,
                body
            )
        );
    }
    else if (is_symbol(arg2)) {
        auto body = t_farm.make_list_cell(next(b, 2), e);
        t_funcs.set_func(
            name,
            t_support.make_nospread_macro_form(
                to_symbol(arg2),
                body
            )
        );
    }
    else {
        throw "nbifunc_defmacro error";
    }
    return arg1;
}

Cell CoreEnvironment::bifunc_set(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) return t_farm.nil();
    const auto& val = (next(b) == e) ? t_farm.nil() : arg2;
    t_support.set_value(arg1, val);
    return val;
}

Cell CoreEnvironment::bifunc_eq(iter b, iter e)
{
    const auto& s1 = (ArgsCounter{b, e} > 0) ? arg1 : t_farm.nil();
    const auto& s2 = (ArgsCounter{b, e} > 1) ? arg2 : t_farm.nil();


    if (is_number(s1) && is_number(s2)) {
        return bool_cast(to_number(s1) == to_number(s2), t_farm);
    }
    else if (is_symbol(s1) && is_symbol(s2)) {
        return bool_cast(to_symbol(s1) == to_symbol(s2), t_farm);
    }
    else if(is_null(s1) && is_null(s2)) {
        return t_farm.T();
    }
    return t_farm.nil();
}

Cell CoreEnvironment::bifunc_last(iter b, iter e)
{
    if (ArgsCounter{b, e}== 0) return t_farm.nil();
    if (is_atom(arg1)) {
        return t_farm.nil();
    }
    else {
        if (is_null_list(to_list(arg1))) {
            return t_farm.nil();
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
    if (ArgsCounter{b, e}== 0) return t_farm.nil();
    return bool_cast(is_integer(arg1), t_farm);
}

Cell CoreEnvironment::bifunc_oblist(iter b, iter e)
{
    const auto& lst = t_farm.get_lst();
    std::vector <Cell> result;
    result.reserve(lst.size());
    for (const auto& symb : lst) {
        result.push_back(t_farm.make_symbol_cell(symb.data()));
    }
    return t_farm.make_list_cell(begin(result), end(result));
}

Cell CoreEnvironment::bifunc_rplaca(iter b, iter e)
{
    Cell s1 = (ArgsCounter{b, e} >= 1) ? arg1 : t_farm.nil();
    Cell s2 = (ArgsCounter{b, e} >= 2) ? arg2 : t_farm.nil();

    if (is_null(s1) || is_number(s1)) {
        return t_farm.nil();
    }
    else if (is_symbol(s1)) {
        t_support.set_value(s1, s2);
        return s1;
    }
    else {
        rplaca(to_list(s1), s2);
        return s1;
    }
}

Cell CoreEnvironment::bifunc_rplacd(iter b, iter e)
{
    Cell s1 = (ArgsCounter{b, e} >= 1) ? arg1 : t_farm.nil();
    Cell s2 = (ArgsCounter{b, e} >= 2) ? arg2 : t_farm.nil();

    if (!is_symbol(s1) && !is_null_list(to_list(s1))) {
        rplacd(to_list(s1), s2);
        return s1;
    }
    return t_farm.nil();

    if (is_null(s1) || is_number(s1)) {
        return t_farm.nil();
    }
    else if (is_symbol(s1)) {
        throw "rplacd: property list not available";
    }
    else {
        rplacd(to_list(s1), s2);
        return s1;
    }
}

Cell CoreEnvironment::bifunc_copy_tree(iter b, iter e)
{
    Cell s1 = (ArgsCounter{b, e} >= 1) ? arg1 : t_farm.nil();
    if (!is_list(s1) || is_null_symbol(s1)) {
        return s1;
    }
    return tree_copy(s1, t_farm);
}

Cell CoreEnvironment::bifunc_pack(iter b, iter e)
{
    if (b == e || !is_list(arg1) || is_null_symbol(arg1)) return t_farm.make_symbol_cell("");
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
    Cell s = t_farm.nil();
    if (b != e) {
        if((is_list(arg1)) && !is_null_symbol(arg1)) return t_farm.nil();
        s = arg1;
    }
    string str = t_output_control.to_string_raw(s);
    vector<Cell> buf;
    buf.reserve(str.size());
    for (char c : str) {
        buf.push_back(t_farm.make_symbol_cell(string() + c));
    }
    return t_farm.make_list_cell(begin(buf), end(buf));
}

Cell CoreEnvironment::bifunc_read_char(iter b, iter e)
{
    auto i = t_input.read_char((ArgsCounter{ b, e } >= 1) ? !is_null(arg1) : false);
    if(i == -1)
        throw "bifunc_read_char: eos";
    return t_farm.make_symbol_cell(string() + char(i));
}

Cell CoreEnvironment::bifunc_unread_char(iter b, iter e)
{
    t_input.unread_char();
    return t_farm.nil();
}
//
//Cell CoreEnvironment::bifunc_peek_char(iter b, iter e)
//{
//    auto ch = t_input.peek_char((ArgsCounter{ b, e } >= 1) ? arg1 : t_farm.nil());
//    cout << "char:" << t_output_control.to_string(ch) << endl;
//    return ch;
//}

Cell CoreEnvironment::bifunc_listen(iter b, iter e)
{
    return bool_cast(t_input.alive(), t_farm);
}

Cell CoreEnvironment::bifunc_break(iter b, iter e)
{
    throw break_helper{ t_output_control.to_string((ArgsCounter{ b, e } >= 1) ? arg1 : t_farm.nil()) };
}

Cell CoreEnvironment::bifunc_get_macro_char(iter b, iter e)
{
    if (b == e || !is_symbol(arg1)) return t_farm.nil();
    auto flag = (ArgsCounter{ b, e } >= 2) ? arg2 : t_farm.nil();
    auto raw = t_output_control.to_string_raw(arg1);
    if(raw.size() > 1) return t_farm.nil();
    if(!t_macrotable.is_macro_char(raw[0])) return t_farm.nil();
    if (auto var = t_macrotable.is_macro_char(raw[0]); var) {
        if (!is_null(flag)) return t_farm.make_symbol_cell("LAMBDA");
        return var->get().func;
    }
    return t_farm.nil();
    
}

Cell CoreEnvironment::bifunc_set_macro_char(iter b, iter e)
{
    if (b == e || !is_symbol(arg1)) return t_farm.nil();
    auto defenition = (ArgsCounter{ b, e } >= 2) ? arg2 : t_farm.nil();
    auto flag = (ArgsCounter{ b, e } >= 3) ? arg3 : t_farm.nil();
    if (!is_list(defenition) || is_null(defenition)) return t_farm.nil();
    auto raw = t_output_control.to_string_raw(arg1);
    if (raw.size() > 1) return t_farm.nil();


    if (is_symbol(flag) && to_symbol(flag) == t_farm.make_symbol("COMMENT")) {
        t_macrotable.set_macro_char(raw[0], macro_char{ macro_char::type_t::comment, defenition });
        //throw "";
    }
    else if(is_null(flag)){
        t_macrotable.set_macro_char(raw[0], macro_char{ macro_char::type_t::terminating, defenition });
    }
    else {
        t_macrotable.set_macro_char(raw[0], macro_char{ macro_char::type_t::non_terminating, defenition });
    }
    
    return t_farm.make_symbol_cell(T_str);
}
