#include "SimpleBifuncs.h"
#include "ArgsCounter.h"
#include "Funcs.h"

#define arg1 *b
#define arg2 *(next(b, 1))
#define arg3 *(next(b, 2))

using namespace std;
using namespace CoreData;


enum class numbers_compare_type { greater, greater_eq, less, less_eq, eq };
template<numbers_compare_type type>
bool numbers_compare(CarCdrIterator b, CarCdrIterator e)
{
    if (ArgsCounter{ b, e } <= 1) return true;
    if (!is_number(*b)) throw "numbers_compare: error";
    const auto* n = &to_number(*b);


    for (auto it = next(b); it != e; ++it) {
        if (!is_number(*it)) throw "numbers_compare: error";
        const auto* n2 = &to_number(*it);

        if constexpr (type == numbers_compare_type::greater) {
            if (!(*n > *n2)) return false;
        }
        if constexpr (type == numbers_compare_type::greater_eq) {
            if (!(*n >= *n2)) return false;
        }
        if constexpr (type == numbers_compare_type::less) {
            if (!(*n < *n2)) return false;
        }
        if constexpr (type == numbers_compare_type::less_eq) {
            if (!(*n <= *n2)) return false;
        }
        if constexpr (type == numbers_compare_type::eq) {
            if (!(*n == *n2)) return false;
        }

        n = n2;
    }

    return true;
}





Cell SBIF::bifunc_atomp(CoreEnvironment& env, iter b, iter e) {
    if (ArgsCounter{ b, e } == 0) return env.farm().T();
    if (is_list(arg1) && is_null_list(to_list(arg1))) return env.farm().T();
    return bool_cast(is_atom(arg1), env.farm());
}

Cell SBIF::bifunc_symbolp(CoreEnvironment& env, iter b, iter e) {
    if (ArgsCounter{ b, e } == 0) return env.farm().T();
    if (is_list(arg1) && is_null_list(to_list(arg1))) return env.farm().T();
    return bool_cast(is_symbol(arg1), env.farm());
}

Cell SBIF::bifunc_numberp(CoreEnvironment& env, iter b, iter e) {
    if (ArgsCounter{ b, e } == 0) return env.farm().nil();
    return bool_cast(is_number((arg1)), env.farm());
}

Cell SBIF::bifunc_listp(CoreEnvironment& env, iter b, iter e) {
    if (ArgsCounter{ b, e } == 0) return env.farm().T();
    return bool_cast(is_list(arg1), env.farm());
}

Cell SBIF::bifunc_null(CoreEnvironment& env, iter b, iter e) {
    if (ArgsCounter{ b, e } == 0) return env.farm().T();
    return bool_cast(is_null((arg1)), env.farm());
}

Cell SBIF::bifunc_add(CoreEnvironment& env, iter b, iter e)
{
    if (ArgsCounter{ b, e } == 0) return env.farm().make_number_cell(BigInt(0l));
    if (!is_number(arg1)) throw "bifunc_add error";
    Number n = to_number(arg1);

    for (auto it = next(b); it != e; ++it) {
        if (!is_number(*it)) throw "bifunc_add error";
        n += to_number(*it);
    }
    return env.farm().make_number_cell(move(n));
}

Cell SBIF::bifunc_sub(CoreEnvironment& env, iter b, iter e)
{
    if (ArgsCounter{ b, e } == 0) throw "bifunc_sub error";
    if (ArgsCounter{ b, e } == 1) {
        if (!is_number(arg1)) throw "bifunc_sub error";
        Number n = to_number(arg1);
        n.minus();
        return  env.farm().make_number_cell(move(n));
    }
    if (!is_number(arg1)) throw "bifunc_sub error";
    Number n = to_number(arg1);

    for (auto it = next(b, 1); it != e; ++it) {
        if (!is_number(*it)) throw "bifunc_sub error";
        n -= to_number(*it);
    }
    return env.farm().make_number_cell(move(n));
}

Cell SBIF::bifunc_mul(CoreEnvironment& env, iter b, iter e)
{
    if (ArgsCounter{ b, e } == 0) {
        return env.farm().make_number_cell(BigInt(1l));
    }
    if (!is_number(arg1)) throw "bifunc_mul error";
    if (ArgsCounter{ b, e } == 1) {
        return arg1;
    }
    Number n = to_number(arg1);

    for (auto it = next(b, 1); it != e; ++it) {
        if (!is_number(*it)) throw "bifunc_mul error";
        n *= to_number(*it);
    }
    return env.farm().make_number_cell(move(n));
}

Cell SBIF::bifunc_div(CoreEnvironment& env, iter b, iter e)
{
    if (ArgsCounter{ b, e } == 0) throw "bifunc_div error";
    if (ArgsCounter{ b, e } == 1) {
        if (!is_number(arg1)) throw "bifunc_div error";
        Number n = env.farm().make_number(1);
        n /= to_number(arg1);
        return env.farm().make_number_cell(move(n));
    }

    if (!is_number(arg1)) throw "bifunc_div error";
    Number n = to_number(arg1);

    if (!is_number(arg2)) throw "bifunc_div error";
    Number n2 = to_number(arg2);

    for (auto it = next(b, 2); it != e; ++it) {
        if (!is_number(*it)) throw "bifunc_div error";
        n2 *= to_number(*it);
    }

    if (n2 == env.farm().make_number(BigInt(0l))) {
        throw "bifunc_div error (Zero Divide)";
    }

    n /= n2;

    return env.farm().make_number_cell(move(n));
}

Cell SBIF::bifunc_greater(CoreEnvironment& env, iter b, iter e)
{
    return bool_cast(numbers_compare<numbers_compare_type::greater>(b, e), env.farm());
}

Cell SBIF::bifunc_greater_equal(CoreEnvironment& env, iter b, iter e)
{
    return bool_cast(numbers_compare<numbers_compare_type::greater_eq>(b, e), env.farm());
}

Cell SBIF::bifunc_less(CoreEnvironment& env, iter b, iter e)
{
    return bool_cast(numbers_compare<numbers_compare_type::less>(b, e), env.farm());
}

Cell SBIF::bifunc_less_equal(CoreEnvironment& env, iter b, iter e)
{
    return bool_cast(numbers_compare<numbers_compare_type::less_eq>(b, e), env.farm());
}

Cell SBIF::bifunc_num_eq(CoreEnvironment& env, iter b, iter e)
{
    return bool_cast(numbers_compare<numbers_compare_type::eq>(b, e), env.farm());
}

Cell SBIF::bifunc_equal(CoreEnvironment& env, iter b, iter e)
{
    const auto& s1 = (ArgsCounter{ b, e } > 0) ? arg1 : env.farm().nil();
    const auto& s2 = (ArgsCounter{ b, e } > 1) ? arg2 : env.farm().nil();
    if (is_number(s1) && is_number(s2)) {
        return bifunc_num_eq(env, b, e);
    }

    if (is_symbol(s1) && is_symbol(s2)) {
        return bifunc_eq(env, b, e);
    }

    if (is_list(s1) && is_list(s2)) {
        reference_wrapper<const Cell> s1_b = s1;
        reference_wrapper<const Cell> s2_b = s2;

        while (!is_null(s1_b) && !is_null(s2_b))
        {
            auto lst = env.farm().make_list_cell({ car(s1_b), car(s2_b) });
            if (
                is_null(bifunc_equal(env, begin(lst), end(lst)))
                )
            {
                return env.farm().nil();
            }
            s1_b = cdr(s1_b);
            s2_b = cdr(s2_b);
        }
        if (is_null(s1_b) && is_null(s2_b)) return env.farm().T();
        auto lst = env.farm().make_list_cell({ car(s1_b), car(s2_b) });
        return bool_cast(
            !is_null(bifunc_equal(env, begin(lst), end(lst))), env.farm()
        );
    }

    if (is_null(s1) && is_null(s2)) {
        return env.farm().T();
    }

    return env.farm().nil();
}

Cell SBIF::bifunc_car(CoreEnvironment& env, iter b, iter e)
{
    Cell s = (ArgsCounter{ b, e } >= 1) ? arg1 : env.farm().nil();

    if (is_atom(s)) {
        return env.core_funcs().eval_atom(s);
    }
    else {
        if (is_null_list(to_list(s))) {
            return env.farm().nil();
        }
        else {
            return car(s);
        }
    }
}

Cell SBIF::bifunc_cdr(CoreEnvironment& env, iter b, iter e)
{
    Cell s = (ArgsCounter{ b, e } >= 1) ? arg1 : env.farm().nil();
    if (is_null(s)) {
        return env.farm().nil();
    }
    else if (is_atom(s)) {
        throw "bifunc_cdr error (property not available)";
    }
    else {
        return cdr(s);
    }
}

Cell SBIF::bifunc_cons(CoreEnvironment& env, iter b, iter e)
{
    Cell s1 = (ArgsCounter{ b, e } >= 1) ? arg1 : env.farm().nil();
    Cell s2 = (ArgsCounter{ b, e } >= 2) ? arg2 : env.farm().nil();

    return cons(s1, s2, env.farm());
}

Cell SBIF::bifunc_list(CoreEnvironment& env, iter b, iter e)
{
    return env.farm().make_list_cell(b, e);
}

Cell SBIF::bifunc_getd(CoreEnvironment& env, iter b, iter e)
{
    if (ArgsCounter{ b, e } == 0) return env.farm().nil();
    if (!is_symbol(arg1)) throw "bifunc_getf error";

    auto func = env.funcs().find(to_symbol(arg1));

    if (auto fnc_r = env.funcs().find(to_symbol(arg1)); fnc_r) {
        auto& data = fnc_r->get();

        if (holds_alternative<FuncsStorage::bifunc>(data) || holds_alternative<FuncsStorage::nbifunc>(data)) {
            return env.farm().nil();
        }
        else if (holds_alternative<lambda>(data)) {
            return gen_cell(get<lambda>(data), env.farm());
        }
        else if (holds_alternative<macro>(data)) {
            return gen_cell(get<macro>(data), env.farm());
        }
    }
    return env.farm().nil();
}

Cell SBIF::bifunc_print(CoreEnvironment& env, iter b, iter e)
{
    if (env.cos()) {
        if (ArgsCounter{ b, e } == 0) {
            (*env.cos()).get().out_new_line(nil_str);
            return env.farm().nil();
        }
        else {
            (*env.cos()).get().out_new_line(env.output_control().to_string(arg1));
            return arg1;
        }
    }
    return env.farm().nil();
}

Cell SBIF::bifunc_prin1(CoreEnvironment& env, iter b, iter e)
{
    if (env.cos()) {
        if (ArgsCounter{ b, e } == 0) {
            (*env.cos()).get().out_without_new_line(nil_str);
            return env.farm().nil();
        }
        else {
            (*env.cos()).get().out_without_new_line(env.output_control().to_string(arg1));
            return arg1;
        }
    }
    return env.farm().nil();
}

Cell SBIF::nbifunc_quote(CoreEnvironment& env, iter b, iter e)
{
    if (ArgsCounter{ b, e } == 0) return env.farm().nil();
    return arg1;
}

Cell SBIF::nbifunc_defun(CoreEnvironment& env, iter b, iter e) {
    if (ArgsCounter{ b, e } < 2)  return env.farm().nil();
    if (!is_symbol(arg1)) return env.farm().nil();
    const auto& name = to_symbol(arg1);
    if (is_list(arg2)) {
        if (is_alambda_form(arg2, env.farm()) || is_null_symbol(arg2))
        {
            env.funcs().set_func(name, env.core_funcs().make_lambda_form(arg2));
        }
        else {
            auto body = env.farm().make_list_cell(next(b, 2), e);
            env.funcs().set_func(
                name,
                env.core_funcs().make_spread_lambda_form(
                    lambda_types::lambda,
                    arg2,
                    body
                )
            );
        }
    }
    else if (is_symbol(arg2)) {
        auto body = env.farm().make_list_cell(next(b, 2), e);
        env.funcs().set_func(
            name,
            env.core_funcs().make_nospread_lambda_form(
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

Cell SBIF::nbifunc_defmacro(CoreEnvironment& env, iter b, iter e)
{
    if (ArgsCounter{ b, e } < 2)  return env.farm().nil();
    if (!is_symbol(arg1)) return env.farm().nil();
    const auto& name = to_symbol(arg1);
    if (is_list(arg2) || is_null_symbol(arg2)) {
        auto body = env.farm().make_list_cell(next(b, 2), e);
        env.funcs().set_func(
            name,
            env.core_funcs().make_spread_macro_form(
                arg2,
                body
            )
        );
    }
    else if (is_symbol(arg2)) {
        auto body = env.farm().make_list_cell(next(b, 2), e);
        env.funcs().set_func(
            name,
            env.core_funcs().make_nospread_macro_form(
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

Cell SBIF::bifunc_set(CoreEnvironment& env, iter b, iter e)
{
    if (ArgsCounter{ b, e } == 0) return env.farm().nil();
    const auto& val = (next(b) == e) ? env.farm().nil() : arg2;
    env.core_funcs().set_value(arg1, val);
    return val;
}

Cell SBIF::bifunc_eq(CoreEnvironment& env, iter b, iter e)
{
    const auto& s1 = (ArgsCounter{ b, e } > 0) ? arg1 : env.farm().nil();
    const auto& s2 = (ArgsCounter{ b, e } > 1) ? arg2 : env.farm().nil();


    if (is_number(s1) && is_number(s2)) {
        return bool_cast(to_number(s1) == to_number(s2), env.farm());
    }
    else if (is_symbol(s1) && is_symbol(s2)) {
        return bool_cast(to_symbol(s1) == to_symbol(s2), env.farm());
    }
    else if (is_null(s1) && is_null(s2)) {
        return env.farm().T();
    }
    return env.farm().nil();
}

Cell SBIF::bifunc_last(CoreEnvironment& env, iter b, iter e)
{
    if (ArgsCounter{ b, e } == 0) return env.farm().nil();
    if (is_atom(arg1)) {
        return env.farm().nil();
    }
    else {
        if (is_null_list(to_list(arg1))) {
            return env.farm().nil();
        }
        auto it = begin(arg1);
        while (next(it) != end(arg1)) {
            ++it;
        }
        return it.get_elem();
    }
}

Cell SBIF::bifunc_length(CoreEnvironment& env, iter b, iter e)
{
    if (ArgsCounter{ b, e } == 0) return env.farm().make_number_cell(BigInt(0l));
    auto& s = arg1;
    if (is_atom(s)) {
        throw "bifunc_length error (wtf ???)";
    }
    else {
        return env.farm().make_number_cell(BigInt(static_cast<long>(std::distance(begin(s), end(s)))));
    }
}

Cell SBIF::bifunc_integerp(CoreEnvironment& env, iter b, iter e)
{
    if (ArgsCounter{ b, e } == 0) return env.farm().nil();
    return bool_cast(is_integer(arg1), env.farm());
}

Cell SBIF::bifunc_oblist(CoreEnvironment& env, iter b, iter e)
{
    const auto& lst = env.farm().get_lst();
    std::vector <Cell> result;
    result.reserve(lst.size());
    for (const auto& symb : lst) {
        result.push_back(env.farm().make_symbol_cell(symb.data()));
    }
    return env.farm().make_list_cell(begin(result), end(result));
}

Cell SBIF::bifunc_rplaca(CoreEnvironment& env, iter b, iter e)
{
    Cell s1 = (ArgsCounter{ b, e } >= 1) ? arg1 : env.farm().nil();
    Cell s2 = (ArgsCounter{ b, e } >= 2) ? arg2 : env.farm().nil();

    if (is_null(s1) || is_number(s1)) {
        return env.farm().nil();
    }
    else if (is_symbol(s1)) {
        env.core_funcs().set_value(s1, s2);
        return s1;
    }
    else {
        rplaca(to_list(s1), s2);
        return s1;
    }
}

Cell SBIF::bifunc_rplacd(CoreEnvironment& env, iter b, iter e)
{
    Cell s1 = (ArgsCounter{ b, e } >= 1) ? arg1 : env.farm().nil();
    Cell s2 = (ArgsCounter{ b, e } >= 2) ? arg2 : env.farm().nil();

    if (!is_symbol(s1) && !is_null_list(to_list(s1))) {
        rplacd(to_list(s1), s2);
        return s1;
    }
    return env.farm().nil();

    if (is_null(s1) || is_number(s1)) {
        return env.farm().nil();
    }
    else if (is_symbol(s1)) {
        throw "rplacd: property list not available";
    }
    else {
        rplacd(to_list(s1), s2);
        return s1;
    }
}

Cell SBIF::bifunc_copy_tree(CoreEnvironment& env, iter b, iter e)
{
    Cell s1 = (ArgsCounter{ b, e } >= 1) ? arg1 : env.farm().nil();
    if (!is_list(s1) || is_null_symbol(s1)) {
        return s1;
    }
    return tree_copy(s1, env.farm());
}

Cell SBIF::bifunc_pack(CoreEnvironment& env, iter b, iter e)
{
    if (b == e || !is_list(arg1) || is_null_symbol(arg1)) return env.farm().make_symbol_cell("");
    std::string s;
    auto it = begin(*b);
    auto e_it = end(*b);
    for (; it != e_it; ++it) {
        if (is_atom(*it)) {
            s += env.output_control().to_string_raw(*it);
        }
    }
    return env.farm().make_symbol_cell(move(s));
}

Cell SBIF::bifunc_unpack(CoreEnvironment& env, iter b, iter e)
{
    Cell s = env.farm().nil();
    if (b != e) {
        if ((is_list(arg1)) && !is_null_symbol(arg1)) return env.farm().nil();
        s = arg1;
    }
    string str = env.output_control().to_string_raw(s);
    vector<Cell> buf;
    buf.reserve(str.size());
    for (char c : str) {
        buf.push_back(env.farm().make_symbol_cell(string() + c));
    }
    return env.farm().make_list_cell(begin(buf), end(buf));
}

Cell SBIF::bifunc_read_char(CoreEnvironment& env, iter b, iter e)
{
    auto i = env.input().read_char((ArgsCounter{ b, e } >= 1) ? !is_null(arg1) : false);
    if (i == -1)
        throw "bifunc_read_char: eos";
    return env.farm().make_symbol_cell(string() + char(i));
}

Cell SBIF::bifunc_unread_char(CoreEnvironment& env, iter b, iter e)
{
    env.input().unread_char();
    return env.farm().nil();
}

Cell SBIF::bifunc_listen(CoreEnvironment& env, iter b, iter e)
{
    return bool_cast(env.input().alive(), env.farm());
}

Cell SBIF::bifunc_break(CoreEnvironment& env, iter b, iter e)
{
    throw break_helper{ env.output_control().to_string((ArgsCounter{ b, e } >= 1) ? arg1 : env.farm().nil()) };
}

Cell SBIF::bifunc_get_macro_char(CoreEnvironment& env, iter b, iter e)
{
    if (b == e || !is_symbol(arg1)) return env.farm().nil();
    auto flag = (ArgsCounter{ b, e } >= 2) ? arg2 : env.farm().nil();
    auto raw = env.output_control().to_string_raw(arg1);
    if (raw.size() > 1) return env.farm().nil();
    if (!env.macrotable().is_macro_char(raw[0])) return env.farm().nil();
    if (auto var = env.macrotable().is_macro_char(raw[0]); var) {
        if (!is_null(flag)) return env.farm().make_symbol_cell("LAMBDA");
        return var->get().func;
    }
    return env.farm().nil();

}

Cell SBIF::bifunc_set_macro_char(CoreEnvironment& env, iter b, iter e)
{
    if (b == e || !is_symbol(arg1)) return env.farm().nil();
    auto defenition = (ArgsCounter{ b, e } >= 2) ? arg2 : env.farm().nil();
    auto flag = (ArgsCounter{ b, e } >= 3) ? arg3 : env.farm().nil();
    if (!is_list(defenition) || is_null(defenition)) return env.farm().nil();
    auto raw = env.output_control().to_string_raw(arg1);
    if (raw.size() > 1) return env.farm().nil();


    if (is_symbol(flag) && to_symbol(flag) == env.farm().make_symbol("COMMENT")) {
        env.macrotable().set_macro_char(raw[0], macro_char{ macro_char::type_t::comment, defenition });
        //throw "";
    }
    else if (is_null(flag)) {
        env.macrotable().set_macro_char(raw[0], macro_char{ macro_char::type_t::terminating, defenition });
    }
    else {
        env.macrotable().set_macro_char(raw[0], macro_char{ macro_char::type_t::non_terminating, defenition });
    }

    return env.farm().T();
}