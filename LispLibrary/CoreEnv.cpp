#include "CoreEnv.h"

#include "BigInt.h"

#include <list>
#include <iostream>
#include <unordered_map>
#include <variant>
#include <cmath>

using namespace std;

Cell CoreEnvironment::nil = make_list({});
Cell CoreEnvironment::T = make_atom(Atom("T"));

CoreEnvironment::CoreEnvironment()
{
	t_vars.emplace("nil", nil);
	t_vars.emplace("T", T);
}

Cell CoreEnvironment::execute_one(const Cell& c, const Mutexted<bool>& stop_flag)
{
    t_stop_flag = &stop_flag;
    Cell result = nil;
    try {
        result = eval(c, t_env);
    }
    catch (const throwhelper&) {
        
    }
    return result; 
}

Cell CoreEnvironment::bool_cast(bool val) {
    if (val)  return T;
    return nil;
}

bool is_null(const Cell& c) {
    if (!c.is_list()) return false;
    return c.to_list().empty();
}

bool is_symbol_c(const Cell& c) {
    if (!c.is_atom()) return false;
    return c.to_atom().is_symbol();
}

bool is_integer_number_c(const Cell& c) {
    if (!c.is_atom()) return false;
    return c.to_atom().is_integer();
}

bool is_real_number_c(const Cell& c) {
    if (!c.is_atom()) return false;
    return c.to_atom().is_real();
}

bool is_number2_c(const Cell& c) {
    if (!c.is_atom()) return false;
    return c.to_atom().is_real() || c.to_atom().is_integer();
}

bool CoreEnvironment::is_T(const Cell& c) {
    if (!c.is_atom()) return false;
    if (!is_symbol_c(c)) return false;
    return c.to_atom().to_symbol() == T.to_atom().to_symbol();
}



Cell CoreEnvironment::bifunc_atomp(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env) {
    return bool_cast(eval(c[1], sub_env).is_atom());
}

Cell CoreEnvironment::bifunc_symbolp(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env) {
    return bool_cast(is_symbol_c(eval(c[1], sub_env)));
}

Cell CoreEnvironment::bifunc_numberp(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env) {
    return bool_cast(is_number2_c(eval(c[1], sub_env)));
}

Cell CoreEnvironment::bifunc_listp(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env) {
    return bool_cast(eval(c[1], sub_env).is_list());
}

Cell CoreEnvironment::bifunc_null(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env) {
    return bool_cast(is_null(eval(c[1], sub_env)));
}

using anumb = variant<double, BigInt>;

void add(anumb& numb, double val) {
    if (holds_alternative<double>(numb)) {
        double sum = get<double>(numb) + val;
        if (abs(sum) < val || abs(sum) < abs(get<double>(numb))) {
            numb = anumb(BigInt(get<double>(numb)));
            get<BigInt>(numb) += BigInt(val);
        }
        else {
            get<double>(numb) += val;
        }
    }
    else {
        if (get<BigInt>(numb).is_castable_to_double()) {
            anumb result = get<BigInt>(numb).to_double();
            add(result,val);
            numb.swap(result);
        }
        else {
            get<BigInt>(numb) += BigInt(val);
        }
    }
}

void add(anumb& numb, const BigInt& val) {
    if (holds_alternative<double>(numb)) {
        anumb buf = val;
        add(buf, get<double>(numb));
        numb.swap(buf);
    }
    else {
        get<BigInt>(numb) += val;
    }
}

void mlt(anumb& numb, double val) {
    if (holds_alternative<double>(numb)) {
        double x = get<double>(numb);

        int exponent1 = 0;
        int exponent2 = 0;
        int exponent3 = 0;

        double significand1 = frexp(x, &exponent1);
        double significand2 = frexp(val, &exponent2);
        double significand3 = frexp(std::numeric_limits<double>::max(), &exponent3);

        if (exponent1 + exponent2 >= exponent3) {
            numb = anumb(BigInt(get<double>(numb)));
            get<BigInt>(numb) *= BigInt(val);
        }
        else {
            get<double>(numb) *= val;
        }
    }
    else {
        if (get<BigInt>(numb).is_castable_to_double()) {
            anumb result = get<BigInt>(numb).to_double();
            mlt(result, val);
            numb.swap(result);
        }
        else {
            get<BigInt>(numb) *= BigInt(val);
        }
    }
}

void mlt(anumb& numb, const BigInt& val) {
    if (holds_alternative<double>(numb)) {
        anumb buf = val;
        mlt(buf, get<double>(numb));
        numb.swap(buf);
    }
    else {
        get<BigInt>(numb) *= val;
    }
}

void div(anumb& numb, double val) {
    if (holds_alternative<double>(numb)) {
        double x = get<double>(numb);

        int exponent1 = 0;
        int exponent2 = 0;
        int exponent3 = 0;

        double significand1 = frexp(x, &exponent1);
        double significand2 = frexp(val, &exponent2);
        double significand3 = frexp(std::numeric_limits<double>::min(), &exponent3);

        if (exponent1 + exponent2 <= exponent3) {
            numb = anumb(BigInt(get<double>(numb)));
            get<BigInt>(numb).div(BigInt(val));
        }
        else {
            get<double>(numb) /= val;
        }
    }
    else {
        if (get<BigInt>(numb).is_castable_to_double()) {
            anumb result = get<BigInt>(numb).to_double();
            div(result, val);
            numb.swap(result);
        }
        else {
            get<BigInt>(numb).div(BigInt(val));
        }
    }
}

void div(anumb& numb, const BigInt& val) {
    if (holds_alternative<double>(numb)) {

        if (val.is_castable_to_double()) {
            numb = get<double>(numb) / val.to_double();
        }
        else {
            anumb result = BigInt(get<double>(numb));
            div(result, val);
            numb.swap(result);
        }
    }
    else {
        get<BigInt>(numb).div(val);
    }
}

bool operator>(const anumb& numb, double val) {
    if (holds_alternative<double>(numb)) {
        return  get<double>(numb) > val;
    }
    else {
        if (get<BigInt>(numb).is_castable_to_double()) {
            return get<BigInt>(numb).to_double() > val;
        }
        else {
            return true;
        }
    }
}

bool operator<(const anumb& numb, double val) {
    if (holds_alternative<double>(numb)) {
        return  get<double>(numb) > val;
    }
    else {
        if (get<BigInt>(numb).is_castable_to_double()) {
            return get<BigInt>(numb).to_double() < val;
        }
        else {
            return false;
        }
    }
}

bool operator==(const anumb& numb, double val) {
    if (holds_alternative<double>(numb)) {
        return  get<double>(numb) == val;
    }
    return false;
}

bool operator>(const anumb& numb, const BigInt& val) {
    if (holds_alternative<double>(numb)) {
        auto buf = get<double>(numb);
        if (get<BigInt>(numb).is_castable_to_double()) {
            return buf > val.to_double();
        }
        else {
            return false;
        }
    }
    else {
        return get<BigInt>(numb) > val;
    }
}

bool operator<(const anumb& numb, const BigInt& val) {
    if (holds_alternative<double>(numb)) {
        auto buf = get<double>(numb);
        if (get<BigInt>(numb).is_castable_to_double()) {
            return buf < val.to_double();
        }
        else {
            return true;
        }
    }
    else {
        return get<BigInt>(numb) < val;
    }
}

bool operator==(const anumb& numb, const BigInt& val) {
    if (holds_alternative<double>(numb)) {
        auto buf = get<double>(numb);
        if (get<BigInt>(numb).is_castable_to_double()) {
            return buf == val.to_double();
        }
        else {
            return false;
        }
    }
    else {
        return get<BigInt>(numb) == val;
    }
}


Cell CoreEnvironment::bifunc_add(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    if(c.size() == 2) throw "bifunc_add error";
    auto buf1 = eval(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_add error";
    anumb n;
    if (buf1.to_atom().is_integer()) {
        n = buf1.to_atom().to_integer();
    }
    else {
        n = buf1.to_atom().to_real();
    }

    for (auto it = begin(c) + 2; it != end(c); ++it) {
        auto buf = eval(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_add error";
        if (buf.to_atom().is_integer()) {
            add(n, buf.to_atom().to_integer());
        }
        else{
            add(n, buf.to_atom().to_real());
        }
    }
    if (holds_alternative<double>(n)) {
        return make_atom(Atom(get<double>(n)));
    }
    return make_atom(Atom(get<BigInt>(n)));
}

Cell CoreEnvironment::bifunc_sub(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    if (c.size() == 2) throw "bifunc_add error";
    auto buf1 = eval(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_add error";
    anumb n;
    if (buf1.to_atom().is_integer()) {
        n = buf1.to_atom().to_integer();
    }
    else {
        n = buf1.to_atom().to_real();
    }

    for (auto it = begin(c) + 2; it != end(c); ++it) {
        auto buf = eval(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_add error";
        if (buf.to_atom().is_integer()) {
            auto reversed = buf.to_atom().to_integer();
            reversed *= BigInt(-1l);
            add(n, reversed);
        }
        else {
            auto reversed = buf.to_atom().to_real();
            reversed *= -1;
            add(n, reversed);
        }
    }
    if (holds_alternative<double>(n)) {
        return make_atom(Atom(get<double>(n)));
    }
    return make_atom(Atom(get<BigInt>(n)));
}

Cell CoreEnvironment::bifunc_mul(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    if (c.size() == 2) throw "bifunc_add error";
    auto buf1 = eval(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_add error";
    anumb n;
    if (buf1.to_atom().is_integer()) {
        n = buf1.to_atom().to_integer();
    }
    else {
        n = buf1.to_atom().to_real();
    }

    for (auto it = begin(c) + 2; it != end(c); ++it) {
        auto buf = eval(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_add error";
        if (buf.to_atom().is_integer()) {
            mlt(n, buf.to_atom().to_integer());
        }
        else {
            mlt(n, buf.to_atom().to_real());
        }
    }
    if (holds_alternative<double>(n)) {
        return make_atom(Atom(get<double>(n)));
    }
    return make_atom(Atom(get<BigInt>(n)));
}

Cell CoreEnvironment::bifunc_div(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    if (c.size() == 1) throw "bifunc_add error";
    auto buf1 = eval(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_add error";

    if (c.size() == 2) {
        anumb result = 1.0;

        if (buf1.to_atom().is_integer()) {
            div(result, buf1.to_atom().to_integer());
        }
        else {
            div(result, buf1.to_atom().to_real());
        }
        if (holds_alternative<double>(result)) {
            return make_atom(Atom(get<double>(result)));
        }
        return make_atom(Atom(get<BigInt>(result)));
    }

    anumb n;
    if (buf1.to_atom().is_integer()) {
        n = buf1.to_atom().to_integer();
    }
    else {
        n = buf1.to_atom().to_real();
    }

    auto buf2 = eval(*(begin(c) + 2), sub_env);
    if (!is_number2_c(buf2)) throw "bifunc_add error";
    
    anumb n1;
    if (buf1.to_atom().is_integer()) {
        n1 = buf2.to_atom().to_integer();
    }
    else {
        n1 = buf2.to_atom().to_real();
    }
    for (auto it = begin(c) + 3; it != end(c); ++it) {
        auto buf = eval(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_add error";
        if (buf.to_atom().is_integer()) {
            mlt(n1, buf.to_atom().to_integer());
        }
        else {
            mlt(n1, buf.to_atom().to_real());
        }
    }

    if (holds_alternative<double>(n1)) {
        div(n, get<double>(n1));
    }
    else {
        div(n, get<BigInt>(n1));
    }

    if (holds_alternative<double>(n)) {
        return make_atom(Atom(get<double>(n)));
    }
    return make_atom(Atom(get<BigInt>(n)));
}

//????
Cell CoreEnvironment::bifunc_greater(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    auto pre_buf1 = eval(c[1], sub_env);
    auto buf2 = eval(c[2], sub_env);
    if (!is_number2_c(pre_buf1) || !is_number2_c(buf2)) throw "bifunc_equal error";

    anumb buf1;
    if (pre_buf1.to_atom().is_integer()) {
        buf1 = pre_buf1.to_atom().to_integer();
    }
    else {
        buf1 = pre_buf1.to_atom().to_real();
    }
    bool result = false;

    if (buf2.to_atom().is_integer()) {
        result = buf1 > buf2.to_atom().to_integer();
    }
    else {
        result = buf1 > buf2.to_atom().to_real();
    }


    if (result) {
        for (auto it = begin(c) + 3; it != end(c); ++it) {
            if (buf2.to_atom().is_integer()) {
                buf1 = buf2.to_atom().to_integer();
            }
            else {
                buf1 = buf2.to_atom().to_real();
            }
            buf2 = eval(*it, sub_env);

            if (!is_number2_c(buf2)) throw "bifunc_equal error";
            if (buf2.to_atom().is_integer()) {
                result = buf1 > buf2.to_atom().to_integer();
            }
            else {
                result = buf1 > buf2.to_atom().to_real();
            }
            if (!result)break;
        }
    }

    return bool_cast(result);
}

//?????
Cell CoreEnvironment::bifunc_less(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    auto pre_buf1 = eval(c[1], sub_env);
    auto buf2 = eval(c[2], sub_env);
    if (!is_number2_c(pre_buf1) || !is_number2_c(buf2)) throw "bifunc_equal error";

    anumb buf1;
    if (pre_buf1.to_atom().is_integer()) {
        buf1 = pre_buf1.to_atom().to_integer();
    }
    else {
        buf1 = pre_buf1.to_atom().to_real();
    }
    bool result = false;

    if (buf2.to_atom().is_integer()) {
        result = buf1 < buf2.to_atom().to_integer();
    }
    else {
        result = buf1 < buf2.to_atom().to_real();
    }


    if (result) {
        for (auto it = begin(c) + 3; it != end(c); ++it) {
            if (buf2.to_atom().is_integer()) {
                buf1 = buf2.to_atom().to_integer();
            }
            else {
                buf1 = buf2.to_atom().to_real();
            }
            buf2 = eval(*it, sub_env);

            if (!is_number2_c(buf2)) throw "bifunc_equal error";
            if (buf2.to_atom().is_integer()) {
                result = buf1 < buf2.to_atom().to_integer();
            }
            else {
                result = buf1 < buf2.to_atom().to_real();
            }
            if (!result)break;
        }
    }

    return bool_cast(result);
}

//????
Cell CoreEnvironment::bifunc_less_equal(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    auto pre_buf1 = eval(c[1], sub_env);
    auto buf2 = eval(c[2], sub_env);
    if (!is_number2_c(pre_buf1) || !is_number2_c(buf2)) throw "bifunc_equal error";

    anumb buf1;
    if (pre_buf1.to_atom().is_integer()) {
        buf1 = pre_buf1.to_atom().to_integer();
    }
    else {
        buf1 = pre_buf1.to_atom().to_real();
    }
    bool result = false;

    if (buf2.to_atom().is_integer()) {
        result = buf1 < buf2.to_atom().to_integer() || buf1 == buf2.to_atom().to_integer();
    }
    else {
        result = buf1 < buf2.to_atom().to_real() || buf1 == buf2.to_atom().to_real();
    }


    if (result) {
        for (auto it = begin(c) + 3; it != end(c); ++it) {
            if (buf2.to_atom().is_integer()) {
                buf1 = buf2.to_atom().to_integer();
            }
            else {
                buf1 = buf2.to_atom().to_real();
            }
            buf2 = eval(*it, sub_env);

            if (!is_number2_c(buf2)) throw "bifunc_equal error";
            if (buf2.to_atom().is_integer()) {
                result = buf1 < buf2.to_atom().to_integer() || buf1 == buf2.to_atom().to_integer();
            }
            else {
                result = buf1 < buf2.to_atom().to_real() || buf1 == buf2.to_atom().to_real();
            }
            if (!result)break;
        }
    }

    return bool_cast(result);
}


//????
Cell CoreEnvironment::bifunc_equal(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    auto pre_buf1 = eval(c[1], sub_env);
    auto buf2 = eval(c[2], sub_env);
    if (!is_number2_c(pre_buf1) || !is_number2_c(buf2)) throw "bifunc_equal error";

    anumb buf1;
    if (pre_buf1.to_atom().is_integer()) {
        buf1 = pre_buf1.to_atom().to_integer();
    }
    else {
        buf1 = pre_buf1.to_atom().to_real();
    }
    bool result = false;

    if (buf2.to_atom().is_integer()) {
        result =  buf1 == buf2.to_atom().to_integer();
    }
    else {
        result = buf1 == buf2.to_atom().to_real();
    }


    if (result) {
        for (auto it = begin(c) + 3; it != end(c); ++it) {
            if (buf2.to_atom().is_integer()) {
                buf1 = buf2.to_atom().to_integer();
            }
            else {
                buf1 = buf2.to_atom().to_real();
            }
            buf2 = eval(*it, sub_env);

            if (!is_number2_c(buf2)) throw "bifunc_equal error";
            if (buf2.to_atom().is_integer()) {
                result = buf1 == buf2.to_atom().to_integer();
            }
            else {
                result = buf1 == buf2.to_atom().to_real();
            }
            if (!result)break;
        }
    }

    return bool_cast(result);
}

Cell CoreEnvironment::bifunc_car(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    auto buf = eval(c[1], sub_env);
    if (!buf.is_list()) return nil;
    for (auto it = begin(c) + 2; it != end(c); ++it) {
        eval(*it, sub_env);
    }
    return buf.to_list()[0];
}

Cell CoreEnvironment::bifunc_cdr(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    auto buf = eval(c[1], sub_env);
    if (!buf.is_list()) return nil;
    for (auto it = begin(c) + 2; it != end(c); ++it) {
        eval(*it, sub_env);
    }
    const auto& lst = buf.to_list();
    return make_list(std::vector<Cell>(next(begin(lst)), end(lst)));
}

Cell CoreEnvironment::bifunc_append(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    std::list<Cell> lst;
    for (auto it = begin(c) + 1; it != end(c); ++it) {
        auto buf = eval(*it, sub_env);
        if (!buf.is_list()) return nil;
        for (const auto& elem : buf.to_list()) {
            lst.push_back(elem);
        }
    }
    return make_list(std::move(lst));
}

Cell CoreEnvironment::bifunc_cons(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    auto buf = eval(c[2], sub_env);
    if (!buf.is_list()) return nil;
    const auto& lst = buf.to_list();
    std::vector<Cell> result(1 + lst.size());
    result[0] = eval(c[1], sub_env);
    for (unsigned char i = 1; i != result.size(); ++i) {
        result[i] = lst[i - 1];
    }
    for (auto it = begin(c) + 3; it != end(c); ++it) {
        eval(*it, sub_env);
    }
    return make_list(std::move(result));
}

Cell CoreEnvironment::bifunc_list(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    std::vector<Cell> lst(c.size() - 1);
    for (unsigned char i = 0; i < c.size() - 1; ++i) {
        lst[i] = eval(c[i + 1], sub_env);
    }
    return make_list(std::move(lst));
}

Cell CoreEnvironment::nbifunc_quote(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    if (next(begin(c)) == end(c)) throw "bifunc_quote";
    return c[1];
}

Cell CoreEnvironment::nbifunc_defun(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env) {
    //FREERAM_PRINT_TEXT( "defun "));
    if (!is_symbol_c(c[1])) return bool_cast(false);
    if (!c[2].is_list()) return bool_cast(false);
    const auto& name = c[1].to_atom().to_symbol();
    const auto& first_lst = c[2].to_list();

    std::vector<Cell> params;
    const Cell* body = nullptr;

    if (first_lst.empty()) {
        if (begin(c) + 3 == end(c)) throw "bifunc_defun";
        body = &(*(begin(c) + 3));
    }
    else {
        //FREERAM_PRINT_TEXT( "w_args "));
        if (!is_symbol_c(first_lst[0])) throw "bifunc_defun";


        const auto& name = first_lst[0].to_atom().to_symbol();
        if (name == "nlambda") {
            if (!first_lst[1].is_list()) throw "bifunc_defun";
            params = first_lst[1].to_list();

            body = &(first_lst[2]);
            /* if (lambdas.find(name) == std::end(lambdas)) {
                 lambdas[name] = make_nlambda(std::move(params), *body);
             }
             else {
                 lambdas.get(name) = make_nlambda(std::move(params), *body);
             }*/
            t_lambdas[name] = make_nlambda(std::move(params), *body);
            return c[1];
        }
        else if (name == "lambda") {
            if (!first_lst[1].is_list()) throw "bifunc_defun";
            params = first_lst[1].to_list();

            body = &(first_lst[2]);
        }
        else {
            params = first_lst;
            body = &(c[3]);
        }
    }
    //FREERAM_PRINT_TEXT( "find ");
    //if (!lambdas.find(name)) {
    //    //FREERAM_PRINT_TEXT( "not found "));
    //    lambdas.add(name, make_lambda(std::move(params), *body));
    //}
    //else {
    //    //FREERAM_PRINT_TEXT( "found "));
    //    lambdas.get(name) = make_lambda(std::move(params), *body);
    //}
    t_lambdas[name] = make_lambda(std::move(params), *body);
    return c[1];
}

Cell CoreEnvironment::eval_atom(const Cell& atom, CoreEnvironment::CellEnv& sub_env) {
    if (is_symbol_c(atom)) {
        if (t_vars.find(atom.to_atom().to_symbol()) != end(t_vars)) {
            return t_vars.at(atom.to_atom().to_symbol());
        }
        else {
            const auto& name = atom.to_atom().to_symbol();
            if (sub_env.find(name) != std::end(sub_env)) {
                return sub_env.at(name);
            }
        }
    }
    return atom;
}

Cell CoreEnvironment::eval(const Cell& arg, CoreEnvironment::CellEnv& sub_env) {
    if (t_stop_flag->get()) throw throwhelper{};
    if (arg.is_list()) {
        const auto& lst = arg.to_list();
        if (!is_symbol_c(lst[0])) throw "eval error";

        {
            auto func = find_bifunc(lst[0].to_atom().to_symbol());
            if (!func.empty()) {
                return func.get()(lst, sub_env);
            }
        }

        if (t_lambdas.find(lst[0].to_atom().to_symbol()) != end(t_lambdas)) {
            CoreEnvironment::CellEnv params;
            const auto& fnc = t_lambdas.at(lst[0].to_atom().to_symbol());
            if (fnc.is_lambda()) {
                {
                    const auto& lambda = fnc.get();
                    auto it = next(begin(lst));
                    for (auto arg_it = begin(lambda.params); arg_it != end(lambda.params); ++arg_it) {
                        params.emplace(arg_it->to_atom().to_symbol(), eval(*it, sub_env));
                        ++it;
                    }

                    for (; it != end(lst); ++it) {
                        eval(*it, sub_env);
                    }
                }
                return eval(fnc.get().body, params);
            }
            else {
                {
                    const auto& lambda = fnc.get();
                    auto it = next(begin(lst));
                    for (auto arg_it = begin(lambda.params); arg_it != end(lambda.params); ++arg_it) {
                        params.emplace(arg_it->to_atom().to_symbol(), *it);
                        ++it;
                    }
                }
                return eval(fnc.get().body, params);
            }
        }
        else {
            throw "eval error";
        }
    }
    else {
        return eval_atom(arg, sub_env);
    }
}

std::pair<bool, Cell>  CoreEnvironment::cond_exec_arc(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env) {
    if (c[0].is_list()) {
        if (is_null(c[0])) return  { false, nil };

        if (is_null(eval(c[0], sub_env))) return  { false, nil };
        else {
            return  { true, eval(c[1], sub_env) };
        }
    }
    else if (is_T(eval(c[0], sub_env))) {
        return { true, eval(c[1], sub_env) };
    }
    else {
        throw "cond_exec_arc";
    }
}

Cell CoreEnvironment::nbifunc_cond(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    for (auto it = next(begin(c)); it != end(c); ++it) {
        const auto& elem = *it;
        if (!elem.is_list()) throw "bifunc_cond";
        auto result_pair = cond_exec_arc(elem.to_list(), sub_env);
        if (result_pair.first) {
            return result_pair.second;
        }
    }
    throw "bifunc_cond";
}

Cell CoreEnvironment::nbifunc_if(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    if (!is_null(eval(c[1], sub_env))) {
        return eval(c[2], sub_env);
    }
    else {
        return eval(c[3], sub_env);
    }
}

Cell CoreEnvironment::nbifunc_progn(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    Cell result;
    for (const auto& now : c) {
        result = eval(now, sub_env);
    }
    return result;
}

Cell CoreEnvironment::nbifunc_setq(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env)
{
    if (!is_symbol_c(c[1])) throw "bifunc_setq";
    const auto& name = c[1].to_atom().to_symbol();
    auto val = eval(c[2], sub_env);
    if (t_vars.find(name) != end(t_vars)) {
        t_vars.erase(name);
        t_vars.emplace(name,val);
    }
    else {
        if (sub_env.find(name) != std::end(sub_env)) {
            sub_env.erase(name);
            sub_env.emplace(name, val);
        }
        else {
            t_vars.emplace(name, val);
        }
    }
    return val;
}

Cell CoreEnvironment::nbifunc_eval(const std::vector<Cell>& c, CoreEnvironment::CellEnv& sub_env) {
    return eval(c[1], sub_env);
}

BIFuncCell CoreEnvironment::find_bifunc(const std::string& str)
{
    if (str == "append") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_append(v, env);}); }
    if (str == "car") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env) {return bifunc_car(v, env);}); }
    if (str == "cdr") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_cdr(v, env);}); }
    if (str == "cons") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_cons(v, env);}); }
    if (str == "list") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_list(v, env);}); }
    if (str == "null") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_null(v, env);}); }
    if (str == "numberp") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_numberp(v, env);}); }
    if (str == "symbolp") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_symbolp(v, env);}); }
    if (str == "listp") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_listp(v, env);}); }
    if (str == "atomp") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_atomp(v, env);}); }

    if (str == "+") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_add(v, env);}); }
    if (str == "-") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_sub(v, env);}); }
    if (str == "*") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_mul(v, env);}); }
    if (str == "/") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_div(v, env);}); }
    if (str == ">") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_greater(v, env);}); }
    if (str == "<") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_less(v, env);}); }
    if (str == "<=") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_less_equal(v, env);}); }
    if (str == "=") { return  make_bifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return bifunc_equal(v, env);}); }

    if (str == "eval") { return  make_nbifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return nbifunc_eval(v, env);}); }
    if (str == "defun") { return  make_nbifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return nbifunc_defun(v, env);}); }
    if (str == "quote") { return  make_nbifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return nbifunc_quote(v, env);}); }
    if (str == "cond") { return  make_nbifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return nbifunc_cond(v, env);}); }
    if (str == "if") { return  make_nbifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return nbifunc_if(v, env);}); }
    if (str == "progn") { return  make_nbifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return nbifunc_progn(v, env);}); }
    if (str == "setq") { return  make_nbifunc([this](const vector<Cell> &v, unordered_map <string, Cell>&env){ return nbifunc_setq(v, env);}); }

    return  {};
}