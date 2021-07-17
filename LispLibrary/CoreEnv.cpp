#include "CoreEnv.h"

#include "BigInt.h"
#include "Funcs.h"

#include <list>
#include <iostream>
#include <unordered_map>
#include <variant>
#include <cmath>
#include <algorithm>

using namespace std;

struct {
    bool operator()(const pair<const char*, CoreEnvironment::bifunc>& lh, const pair<const char*, CoreEnvironment::bifunc>& rh)const {
        return strcmp(lh.first, rh.first) < 0;
    }
} bifunc_pair_less;

struct {
    bool operator()(const pair<const char*, CoreEnvironment::bifunc>& lh, const string& rh)const {
        return lh.first < rh;
    }
} bifunc_pair_less_w_string;


LambdaCell make_lambda_form(
    bool lambda,
    Cell::olist::const_iterator beg_params,
    Cell::olist::const_iterator end_params,
    Cell::olist::const_iterator beg_body,
    Cell::olist::const_iterator end_body
)
{
    auto size = std::distance(beg_params, end_params);
    std::vector<std::string> params(size);
    size_t i = 0;
    for (auto it = beg_params; it != end_params; ++it, ++i) {
        if (!is_symbol_c(*it)) throw "get_lambda_form error";
        params[i] = it->to_atom().to_symbol();
    }

    if (lambda) {
        return  make_lambda(move(params), { beg_body , end_body });
    }
    else {
        return make_nlambda(move(params), { beg_body , end_body });
    }
}


LambdaCell get_lambda_form(
    Cell::olist::const_iterator beg_it,
    Cell::olist::const_iterator end_it,
    CellEnvironment& sub_env
)
{
    if (std::distance(beg_it, end_it) < 2) throw "get_lambda_form error";
    if (!is_symbol_c(*beg_it)) throw "get_lambda_form error";
    if (
        beg_it->to_atom().to_symbol() == CoreEnvironment::nlambda_str
        ||
        beg_it->to_atom().to_symbol() == CoreEnvironment::lambda_str
        )
    {
        if (!(beg_it + 1)->is_list()) {
            // (defun name x body) stil not available
            throw "get_lambda_form error (not available)";
        }
        return
            make_lambda_form(
                beg_it->to_atom().to_symbol() == CoreEnvironment::lambda_str,
                begin((beg_it + 1)->to_list()), end((beg_it + 1)->to_list()),
                beg_it + 2, end_it
            );
    }
    throw "get_lambda_form error";
}



CoreEnvironment::bifuncs_array bifunc_setup() {
    auto result =  CoreEnvironment::bifuncs_array{
        make_pair("CAR", &CoreEnvironment::bifunc_car),
        make_pair("CDR", &CoreEnvironment::bifunc_cdr),
        make_pair("CONS", &CoreEnvironment::bifunc_cons),
        make_pair("LIST", &CoreEnvironment::bifunc_list),
        make_pair("NULL", &CoreEnvironment::bifunc_null),
        make_pair("NUMBERP", &CoreEnvironment::bifunc_numberp),
        make_pair("SYMBOLP", &CoreEnvironment::bifunc_symbolp),
        make_pair("LISTP", &CoreEnvironment::bifunc_listp),
        make_pair("ATOM", &CoreEnvironment::bifunc_atomp),
        make_pair("+", &CoreEnvironment::bifunc_add),
        make_pair("-", &CoreEnvironment::bifunc_sub),
        make_pair("*", &CoreEnvironment::bifunc_mul),
        make_pair("/", &CoreEnvironment::bifunc_div),
        make_pair(">", &CoreEnvironment::bifunc_greater),
        make_pair("<", &CoreEnvironment::bifunc_less),
        make_pair("<=", &CoreEnvironment::bifunc_less_equal),
        make_pair(">=", &CoreEnvironment::bifunc_greater_equal),
        make_pair("=", &CoreEnvironment::bifunc_equal),
        make_pair("GETD", &CoreEnvironment::bifunc_getd),
        make_pair("EVAL", &CoreEnvironment::nbifunc_eval),
        make_pair("DEFUN", &CoreEnvironment::nbifunc_defun),
        make_pair("QUOTE", &CoreEnvironment::nbifunc_quote),
        make_pair("COND", &CoreEnvironment::nbifunc_cond),
        make_pair("PROGN", &CoreEnvironment::nbifunc_progn),
        make_pair("SET", &CoreEnvironment::bifunc_set),
        make_pair("SETQ", &CoreEnvironment::nbifunc_setq),
        make_pair("READ", &CoreEnvironment::bifunc_read),
        make_pair("PROG1", &CoreEnvironment::bifunc_prog1),
        make_pair("LOOP", &CoreEnvironment::nbifunc_loop),
        make_pair("PRINT", &CoreEnvironment::bifunc_print),
        make_pair("EQUAL", &CoreEnvironment::bifunc_print),
        make_pair("APPLY", &CoreEnvironment::bifunc_apply),
        make_pair("LAST", &CoreEnvironment::bifunc_last),
        make_pair("LENGTH", &CoreEnvironment::bifunc_length),
        //make_pair("PACK", &CoreEnvironment::bifunc_pack),
        //make_pair("UNPACK", &CoreEnvironment::bifunc_unpack),
        make_pair("EQ", &CoreEnvironment::bifunc_eq),
       // make_pair("MOD", &CoreEnvironment::bifunc_mod),
    };

    std::sort(
        begin(result),
        end(result),
        bifunc_pair_less
    );

    return result;
}


const char* CoreEnvironment::nil_str = "NIL";
const char* CoreEnvironment::T_str = "T";
const char* CoreEnvironment::read_up_case_str = "*READ-UPCASE*";

const char* CoreEnvironment::lambda_str = "LAMBDA";
const char* CoreEnvironment::nlambda_str = "NLAMBDA";

const Cell CoreEnvironment::nil = make_list({});
const Cell CoreEnvironment::T = make_atom(Atom(T_str));

const  CoreEnvironment::bifuncs_array  CoreEnvironment::bifuncs = bifunc_setup();


CoreEnvironment::CoreEnvironment(std::istream& input, std::ostream& output):
    CoreEnvStreamsProvider(input, output),
    t_env({})
{
	t_vars.emplace(nil_str, nil);
	t_vars.emplace(T_str, T);


    //t_vars.emplace("*READ-UPCASE*", T);
}

Cell CoreEnvironment::execute_one(const Cell& c, const IMutexed<bool>& stop_flag)
{
    t_stop_flag = { stop_flag };
    return eval_quote(c, t_env);
}

const std::unordered_map<std::string, LambdaCell>& CoreEnvironment::lambdas() const
{
    return t_lambdas;
}

const std::unordered_map<std::string, Cell>& CoreEnvironment::vars() const
{
    return t_vars;
}

Cell CoreEnvironment::bifunc_atomp(const std::vector<Cell>& c, CellEnvironment& sub_env) {
    return bool_cast(eval_quote(c[1], sub_env).is_atom());
}

Cell CoreEnvironment::bifunc_symbolp(const std::vector<Cell>& c, CellEnvironment& sub_env) {
    return bool_cast(is_symbol_c(eval_quote(c[1], sub_env)));
}

Cell CoreEnvironment::bifunc_numberp(const std::vector<Cell>& c, CellEnvironment& sub_env) {
    return bool_cast(is_number2_c(eval_quote(c[1], sub_env)));
}

Cell CoreEnvironment::bifunc_listp(const std::vector<Cell>& c, CellEnvironment& sub_env) {
    return bool_cast(eval_quote(c[1], sub_env).is_list());
}

Cell CoreEnvironment::bifunc_null(const std::vector<Cell>& c, CellEnvironment& sub_env) {
    return bool_cast(is_null(eval_quote(c[1], sub_env)));
}

Cell CoreEnvironment::bifunc_add(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() == 1) return make_atom(Atom(Number(BigInt(0l))));
    auto buf1 = eval_quote(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_add error";
    Number n = buf1.to_atom().to_number();

    for (auto it = begin(c) + 2; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_add error";
        n += buf.to_atom().to_number();
    }
    return make_atom(Atom(n));
}

Cell CoreEnvironment::bifunc_sub(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() < 2) throw "bifunc_sub error";

    if (c.size() == 2) {
        auto buf1 = eval_quote(*(begin(c) + 1), sub_env);
        if (!is_number2_c(buf1)) throw "bifunc_sub error";
        Number n = buf1.to_atom().to_number();
        n.minus();
        return make_atom(Atom(n));
    }
    auto buf1 = eval_quote(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_sub error";
    Number n = buf1.to_atom().to_number();

    for (auto it = begin(c) + 2; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_sub error";
        n -= buf.to_atom().to_number();
    }
    return make_atom(Atom(n));
}

Cell CoreEnvironment::bifunc_mul(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() < 1) throw "bifunc_mul error";

    if (c.size() == 1) {
        return make_atom(Atom(Number(BigInt(1l))));
    }

    if (c.size() == 2) {
        auto buf1 = eval_quote(*(begin(c) + 1), sub_env);
        if (!is_number2_c(buf1)) throw "bifunc_mul error";
        return buf1;
    }
    auto buf1 = eval_quote(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_mul error";
    Number n = buf1.to_atom().to_number();

    for (auto it = begin(c) + 2; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_mul error";
        n *= buf.to_atom().to_number();
    }
    return make_atom(Atom(n));
}

Cell CoreEnvironment::bifunc_div(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() < 2) throw "bifunc_div error";

    if (c.size() == 2) {
        auto buf1 = eval_quote(*(begin(c) + 1), sub_env);
        if (!is_number2_c(buf1)) throw "bifunc_div error";
        Number n(1);
        n /= buf1.to_atom().to_number();
        return make_atom(Atom(n));
    }

    auto buf1 = eval_quote(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_div error";
    Number n = buf1.to_atom().to_number();

    auto buf2 = eval_quote(*(begin(c) + 2), sub_env);
    if (!is_number2_c(buf2)) throw "bifunc_div error";
    Number n2 = buf2.to_atom().to_number();

    for (auto it = begin(c) + 3; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_div error";
        n2 *= buf.to_atom().to_number();
    }

    if (n2 == Number(BigInt(0l))) {
        throw "bifunc_div error (Zero Divide)";
    }

    n /= n2;

    return make_atom(Atom(n));
}

//Cell CoreEnvironment::bifunc_mod(const std::vector<Cell>& c, CellEnvironment& sub_env)
//{
//    if (c.size() < 3) throw "bifunc_mod error";
//    auto q = eval_quote(*(begin(c) + 1), sub_env);
//    auto w = eval_quote(*(begin(c) + 2), sub_env);
//    if(!is_number2_c(q) || !is_number2_c(w))throw "bifunc_div error (Nonnumeric Argument)";
//    if (w.to_atom().to_number() == Number(BigInt(0l)))  throw "bifunc_div error (Zero Divide)";
//
//    auto result = q.to_atom().to_number().mod(w.to_atom().to_number());
//    eval_rest(begin(c) + 3, end(c), sub_env);
//    return make_atom(Atom(result));
//}


Cell CoreEnvironment::bifunc_greater(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() < 3) throw "bifunc_greater error";
    auto buf1 = eval_quote(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_greater error";
    Number n = buf1.to_atom().to_number();

    bool result = true;
    auto it = begin(c) + 2;
    for (; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_greater error";
        Number n2 = buf.to_atom().to_number();
        result = n > n2;

        if (!result) break;
        n = move(n2);
    }

    for (; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_greater error";
    }
    return bool_cast(result);
}

Cell CoreEnvironment::bifunc_greater_equal(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() < 3) throw "bifunc_greater_equal error";
    auto buf1 = eval_quote(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_greater_equal error";
    Number n = buf1.to_atom().to_number();

    bool result = true;
    auto it = begin(c) + 2;
    for (; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_greater_equal error";
        Number n2 = buf.to_atom().to_number();
        result = n >= n2;

        if (!result) break;
        n = move(n2);
    }

    for (; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_greater_equal error";
    }
    return bool_cast(result);
}

Cell CoreEnvironment::bifunc_less(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() < 3) throw "bifunc_less error";
    auto buf1 = eval_quote(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_less error";
    Number n = buf1.to_atom().to_number();

    bool result = true;
    auto it = begin(c) + 2;
    for (; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_less error";
        Number n2 = buf.to_atom().to_number();
        result = n < n2;

        if (!result) break;
        n = move(n2);
    }

    for (; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_less error";
    }

    return bool_cast(result);
}

Cell CoreEnvironment::bifunc_less_equal(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() < 3) throw "bifunc_less_equal error";
    auto buf1 = eval_quote(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_less_equal error";
    Number n = buf1.to_atom().to_number();

    bool result = true;
    auto it = begin(c) + 2;
    for (; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_less_equal error";
        Number n2 = buf.to_atom().to_number();
        result = n <= n2;

        if (!result) break;
        n = move(n2);
    }

    for (; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_less_equal error";
    }

    return bool_cast(result);
}

Cell CoreEnvironment::bifunc_equal(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() < 3) throw "bifunc_equal error";
    auto buf1 = eval_quote(*(begin(c) + 1), sub_env);
    if (!is_number2_c(buf1)) throw "bifunc_equal error";
    Number n = buf1.to_atom().to_number();

    bool result = true;
    auto it = begin(c) + 2;
    for (; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_equal error";
        Number n2 = buf.to_atom().to_number();
        result = n == n2;

        if (!result) break;
        n = move(n2);
    }

    for (; it != end(c); ++it) {
        auto buf = eval_quote(*it, sub_env);
        if (!is_number2_c(buf)) throw "bifunc_equal error";
    }

    return bool_cast(result);
}

Cell CoreEnvironment::bifunc_car(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    Cell s = nil;
    Cell result;
    if (c.size() >= 2) {
        s = eval_quote(c[1], sub_env);
    }

    if (s.is_atom()) {
        result = eval_quote(s, sub_env);
    }
    else if (s.is_list()) {
        const auto& lst = s.to_list();
        if (lst.empty()) {
            result = nil;
        }
        else {
            result = lst[0];
        }
    }
    else {
        throw "bifunc_car error (dot pair not available)";
    }
    eval_rest(begin(c) + 1 + (c.size() >= 2 ? 1 : 0), end(c), sub_env);
    return result;
}

Cell CoreEnvironment::bifunc_cdr(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    Cell s = nil;
    Cell result;
    if (c.size() >= 2) {
        s = eval_quote(c[1], sub_env);
    }

    if (s.is_atom()) {
        throw "bifunc_cdr error (property not available)";
    }
    else if (s.is_list()) {
        const auto& lst = s.to_list();
        if (lst.empty()) {
            result = nil;
        }
        else {
            result = lst;
            result.to_list().erase(begin(result.to_list()));
        }
    }
    else {
        throw "bifunc_cdr error (dot pair not available)";
    }
    eval_rest(begin(c) + 1 + (c.size() >= 2 ? 1 : 0), end(c), sub_env);
    return result;
}

//Cell CoreEnvironment::bifunc_append(const std::vector<Cell>& c, CellEnvironment& sub_env)
//{
//    if (c.size() < 3) throw "bifunc_append error";
//    std::list<Cell> lst;
//    for (auto it = begin(c) + 1; it != end(c); ++it) {
//        auto buf = eval_quote(*it, sub_env);
//        if (!buf.is_list()) return nil;
//        for (const auto& elem : buf.to_list()) {
//            lst.push_back(elem);
//        }
//    }
//    return make_list(std::move(lst));
//}

Cell CoreEnvironment::bifunc_cons(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    Cell s1;
    Cell s2;
    auto count = c.size() - 1;
    if (count == 0) {
        s1 = nil;
        s2 = nil;
    }
    else if (count == 1) {
        s2 = nil;
    } 
    else {
        s1 = eval_quote(c[1], sub_env);
        s2 = eval_quote(c[2], sub_env);
    }

    if (s2.is_atom()) {
        throw "bifunc_cons error (dot pair is not available)";
    }

    Cell::olist result = s2.to_list();
    if (result.empty()) {
        result = { s1 };
    }
    else {
        result.insert(begin(result), eval_quote(c[1], sub_env));
    }

    eval_rest(begin(c) + 1 + (count > 2 ? (count - 2) : count), end(c), sub_env);
    return make_list(std::move(result));
}

Cell CoreEnvironment::bifunc_list(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    std::vector<Cell> lst(c.size() - 1);
    for (unsigned char i = 0; i < c.size() - 1; ++i) {
        lst[i] = eval_quote(c[i + 1], sub_env);
    }
    return make_list(std::move(lst));
}

Cell CoreEnvironment::bifunc_getd(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() < 2) throw "bifunc_getf error";
    auto buf = eval_quote(c[1], sub_env);
    if(!is_symbol_c(buf)) throw "bifunc_getf error";

    Cell result;

    if (is_bifunc(buf.to_atom().to_symbol())) {
        result = nil;
    }
    else {
        if (t_lambdas.find(buf.to_atom().to_symbol()) != end(t_lambdas)) {
            const auto& fnc = t_lambdas.at(buf.to_atom().to_symbol());

            if (fnc.is_lambda()) {
                const auto& lambda = fnc.get();
                string str = "(";
                str += lambda_str;
                str += " (";

                if (!lambda.params.empty()){
                    for (const auto& name : lambda.params) {
                        str += name + " ";
                    }
                    str.erase(str.size() - 1);
                }
                str += ") ";
                if (lambda.body.size()) {
                    str += to_string(lambda.body[0]);
                }
                else {
                    str += to_string(lambda.body);
                }
                str += ")";
                result = make_atom(Atom(move(str)));
            }
            else if(fnc.is_nlambda()){
                const auto& lambda = fnc.get();
                string str = "(";
                str += nlambda_str;
                str += " (";

                if (!lambda.params.empty()) {
                    for (const auto& name : lambda.params) {
                        str += name + " ";
                    }
                    str.erase(str.size() - 1);
                }
                str += ") ";
                if (lambda.body.size()) {
                    str += to_string(lambda.body[0]);
                }
                else {
                    str += to_string(lambda.body);
                }
                str += ")";
                result = make_atom(Atom(move(str)));
            }
        }
        else {
            result = nil;
        }
    }


    eval_rest(begin(c) + 2, end(c), sub_env);
    return  result;
}

// eval rest????
Cell CoreEnvironment::bifunc_read(const std::vector<Cell>& c, CellEnvironment& sub_env) {
    for (const auto& cell : c) {
        eval_quote(cell, sub_env);
    }

    for(;;){
        auto [result_reason, cell] 
            = read_one_expression_from_stream(
                t_input_stream(),
                ReferencePublicCoreEnvironmentProvider(*this),
                stream_read_mode::new_string,
                true
            );
        if (result_reason) { return cell; }
        if (t_stop_flag) throw throw_stop_helper{};
    }
}

Cell CoreEnvironment::bifunc_prog1(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() == 1) return nil;
    Cell result = eval_quote(c[1], sub_env);
    nbifunc_progn_modif(begin(c) + 2, end(c), sub_env);
    return result;
}

Cell CoreEnvironment::nbifunc_loop(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    auto it = begin(c) + 1;
    for(;;) {
        if(it == end(c)) it = begin(c) + 1;
        if (is_implicit_cond(*it)) {
            auto buf = eval_quote(it->to_list()[0], sub_env);
            if (is_symbol_c(buf) && buf.to_atom().to_symbol() == T_str) {
                if (it->to_list().size() == 1) return nil;
                return nbifunc_progn_modif(next(begin(it->to_list())), end(it->to_list()), sub_env);
            }
        }
        else {
            eval_quote(*it, sub_env);
        }
        ++it;
    }
}


Cell CoreEnvironment::bifunc_print(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() < 2) throw "bifunc_print error";
    auto result = eval_quote(c[1], sub_env);
    t_output_stream() << to_string(result) << endl;
    eval_rest(begin(c) + 2, end(c), sub_env);
    return result;
}

Cell CoreEnvironment::nbifunc_quote(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() < 2) throw "bifunc_quote error";
    return c[1];
}

LambdaCell make_lambda_from(bool lambda, const std::vector<Cell>& param_cells, std::vector<Cell> body) {
    std::vector<std::string> params(param_cells.size());
    for (size_t i = 0; i < param_cells.size(); ++i) {
        if (!is_symbol_c(param_cells[i])) throw "make_lambda_from error";
        params[i] = param_cells[i].to_atom().to_symbol();
    }

    if (lambda) {
        return  make_lambda(move(params), move(body));
    }
    else {
        return make_nlambda(move(params), move(body));
    }
}

Cell CoreEnvironment::nbifunc_defun(const std::vector<Cell>& c, CellEnvironment& sub_env) {
    if (c.size() < 3)  return bool_cast(false);
    if (!is_symbol_c(c[1])) return bool_cast(false);
    if (!c[2].is_list()) {
        throw "(defun name x body) stil not available";
    }
    const auto& name = c[1].to_atom().to_symbol();
    if (c[2].to_list().empty()) {
        t_lambdas[name] = make_lambda_form(true, end(c), end(c), begin(c) + 3, end(c));
        return c[1];
    }
    const auto& lst = c[2].to_list();
    if (!is_symbol_c(lst[0])) throw "bifunc_defun error";
    if (lst[0].to_atom().to_symbol() == nlambda_str || lst[0].to_atom().to_symbol() == lambda_str) {
        t_lambdas[name] = get_lambda_form(begin(lst), end(lst), sub_env);
    }
    else {
        t_lambdas[name] = make_lambda_form(true, begin(lst), end(lst), begin(c) + 3, end(c));
    }
    return c[1];
}

Cell CoreEnvironment::eval_atom(const Cell& atom, CellEnvironment& sub_env) {
    if (is_symbol_c(atom)) {
        if (t_vars.find(atom.to_atom().to_symbol()) != end(t_vars)) {
            return t_vars.at(atom.to_atom().to_symbol());
        }
        else {
            const auto& name = atom.to_atom().to_symbol();
            auto val = sub_env.get(name);
            if (val) {
                return val->get();
            }
        }
    }
    return atom;
}

Cell CoreEnvironment::eval_lambda(
    const LambdaCell& fnc,
    Cell::olist::const_iterator beg_it,
    Cell::olist::const_iterator end_it,
    CellEnvironment& sub_env
) {
    std::unordered_map<std::string, Cell> params;
    if (fnc.is_lambda()) {
        const auto& lambda = fnc.get();
        auto it = beg_it;
        for (auto arg_it = begin(lambda.params); arg_it != end(lambda.params); ++arg_it) {
            if (it == end_it) {
                params.emplace(*arg_it, nil);
            }
            else {
                params.emplace(*arg_it, eval_quote(*it, sub_env));
                ++it;
            }
        }

        eval_rest(it, end_it, sub_env);
    }
    else {
        const auto& lambda = fnc.get();
        auto it = beg_it;
        for (auto arg_it = begin(lambda.params); arg_it != end(lambda.params); ++arg_it) {
            if (it == end_it) {
                params.emplace(*arg_it, nil);
            }
            else {
                params.emplace(*arg_it, *it);
                ++it;
            }
        }

    }
    const auto& body = fnc.get().body;
    CellEnvironment params_env(params, sub_env);
    return nbifunc_progn_modif(begin(body), end(body), params_env);
}

std::pair<bool, Cell> CoreEnvironment::try_bifunc(const std::string& str, const std::vector<Cell>& v, CellEnvironment& env)
{
    auto it =
        std::lower_bound(
            begin(bifuncs),
            end(bifuncs),
            str,
            bifunc_pair_less_w_string
    );
    if (it != end(bifuncs) && (it->first == str)) return { true,  (this->*(it->second))(v, env) };
    return { false, Cell() };
}

bool CoreEnvironment::is_bifunc(const std::string& str)
{
    auto it = 
        std::lower_bound(
            begin(bifuncs),
            end(bifuncs),
            str,
            bifunc_pair_less_w_string
        );
    if (it != end(bifuncs) && (it->first == str)) return true;
    return false;
}

Cell CoreEnvironment::nbifunc_progn_modif(Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, CellEnvironment& sub_env)
{
    if (beg_it == end_it) return nil;
    for (auto it = beg_it; it != prev(end_it); ++it) {
        if (is_implicit_cond(*it)) {
            auto predicate_val = eval_quote(it->to_list()[0], sub_env);
            if (is_symbol_c(predicate_val) && predicate_val.to_atom().to_symbol() == T_str) {
                if (it->to_list().size() == 1) return predicate_val;
                return nbifunc_progn_modif(next(begin(it->to_list())), end(it->to_list()), sub_env);
            }
        }
        else {
            eval_quote(*it, sub_env);
        }
    }

    auto it = prev(end_it);
    if (is_implicit_cond(*it)) {
        auto predicate_val = eval_quote(it->to_list()[0], sub_env);
        if (is_symbol_c(predicate_val) && predicate_val.to_atom().to_symbol() == T_str) {
            if (it->to_list().size() == 1) return predicate_val;
            return nbifunc_progn_modif(next(begin(it->to_list())), end(it->to_list()), sub_env);
        }
    }
    else {
        return eval_quote(*it, sub_env);
    }
}

Cell CoreEnvironment::eval_quote(const Cell& arg, CellEnvironment& sub_env) {
    if ((t_stop_flag) && (*t_stop_flag).get().get()) throw throw_stop_helper{};
    if (arg.is_list()) {
        const auto& lst = arg.to_list();
        if (lst.empty()) return nil;
        if (!is_symbol_c(lst[0])) {
            if (lst[0].is_list())
            {
                return eval_lambda(
                        get_lambda_form(begin(lst[0].to_list()), end(lst[0].to_list()), sub_env),
                        begin(lst) + 1,
                        end(lst),
                        sub_env
                    );
            }
            throw "eval error";
        }

        {
            auto [reason, result] = try_bifunc(lst[0].to_atom().to_symbol(), lst, sub_env);
            if (reason) {
                return result;
            }
        }

        if (t_lambdas.find(lst[0].to_atom().to_symbol()) != end(t_lambdas)) {
            return eval_lambda(t_lambdas.at(lst[0].to_atom().to_symbol()), begin(lst) + 1, end(lst), sub_env);
        }
        else {
            throw "eval error";
        }
    }
    else {
        return eval_atom(arg, sub_env);
    }
}

void CoreEnvironment::eval_rest(Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, CellEnvironment& sub_env)
{
    for (auto it = beg_it; it != end_it; ++it) {
        eval_quote(*it, sub_env);
    }
}

Cell CoreEnvironment::nbifunc_cond(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    for (auto it = begin(c) + 1; it != end(c); ++it) {
        const auto& elem = *it;
        if (!elem.is_list()) throw "bifunc_cond error";

        const auto& lst = elem.to_list();
        auto predicate_val = eval_quote(lst[0], sub_env);
        if (is_symbol_c(predicate_val) && predicate_val.to_atom().to_symbol() == T_str) {
            if (it->to_list().size() == 1) return predicate_val;
            return nbifunc_progn_modif(next(begin(lst)), end(lst), sub_env);
        }
    }
    return nil;
}

Cell CoreEnvironment::nbifunc_progn(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    return nbifunc_progn_modif(next(begin(c)), end(c), sub_env);
}

Cell CoreEnvironment::bifunc_set(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    return bifunc_set_modif(next(begin(c)), end(c), sub_env);
}

Cell CoreEnvironment::bifunc_eq(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() == 1) return nil;
    if (c.size() == 2) return eval_quote(c[1], sub_env);
    auto s1 = eval_quote(c[1], sub_env);
    auto s2 = eval_quote(c[2], sub_env);
    Cell result;

    if (!s1.is_atom() || !s2.is_atom()) {
        result = nil;
    }
    else if (s1.to_atom().is_number() && s2.to_atom().is_number()) {
        result = bool_cast(s1.to_atom().to_number() == s2.to_atom().to_number());
    }
    else if (s1.to_atom().is_symbol() && s2.to_atom().is_symbol()) {
        result = bool_cast(s1.to_atom().to_symbol() == s2.to_atom().to_symbol());
    }

    eval_rest(begin(c) + 3, end(c), sub_env);
    return result;
}

Cell CoreEnvironment::bifunc_last(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() == 1) return nil;
    auto el = eval_quote(c[1], sub_env);
    Cell result;
    if (el.is_atom()) {
        result = nil;
    }
    else if(el.is_list()){
        const auto& lst = el.to_list();
        if (lst.empty()) return nil;
        if (!lst.back().is_list() && !lst.back().is_atom()) {
            throw "bifunc_last error (dot pair not available)";
        }
        else {
            result = make_list({ lst.back() });
        }
    } 
    else {
        throw "bifunc_last error (dot pair not available)";
    }
    eval_rest(begin(c) + 2, end(c), sub_env);
    return result;
}

Cell CoreEnvironment::bifunc_length(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() == 1) return make_atom(Atom(Number(BigInt(0l))));
    auto s = eval_quote(c[1], sub_env);
    Cell result;
    if (s.is_atom()) {
        throw "bifunc_length error (wtf ???)";
    }
    else if (s.is_list()) {
        result = make_atom(Atom(Number(BigInt(static_cast<long>(s.to_list().size())))));
    }
    else {
        throw "bifunc_length error (dot pair not available)";
    }

    eval_rest(begin(c) + 2, end(c), sub_env);
    return result;
 }

Cell CoreEnvironment::bifunc_apply(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    if (c.size() == 1) {
        throw "bifunc_apply error (unknown function)";
    }
    auto fnc = eval_quote(c[1], sub_env);
    Cell result;
    auto it = begin(c) + 2;
    if (c.size() >= 3) {
        ++it;
        auto lst_cell = eval_quote(c[2], sub_env);
        if(lst_cell.is_list()) {
            auto lst = lst_cell.to_list();
            lst.insert(begin(lst), fnc);
            result = eval_quote(make_list(move(lst)), sub_env);
        }
        else {
            result = eval_quote(make_list({ fnc }), sub_env);
        }
    }
    else {
        result = eval_quote(make_list({fnc}), sub_env);
    }

    eval_rest(it, end(c), sub_env);
    return result;
}

Cell CoreEnvironment::nbifunc_setq(const std::vector<Cell>& c, CellEnvironment& sub_env)
{
    return bifunc_setq_modif(next(begin(c)), end(c), sub_env);
}

Cell CoreEnvironment::nbifunc_eval(const std::vector<Cell>& c, CellEnvironment& sub_env) {
    if (c.size() == 1) return nil;
    auto result = eval_quote(eval_quote(c[1], sub_env), sub_env);
    eval_rest(begin(c) + 2, end(c), sub_env);
    return result;
}

Cell CoreEnvironment::bifunc_set_modif(Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, CellEnvironment& sub_env)
{
    if (beg_it == end_it) return nil;
    auto name_cell = eval_quote(*beg_it, sub_env);
    if (!is_symbol_c(name_cell)) throw "bifunc_set error";
    const auto& name = name_cell.to_atom().to_symbol();
    auto it = beg_it + 1;
    auto val = (it == end_it) ? nil : eval_quote(*it, sub_env);

    if (t_vars.find(name) != end(t_vars)) {
        t_vars.erase(name);
        t_vars.emplace(name, val);
    }
    else {
        auto env_val = sub_env.get(name);
        if (env_val) {
            env_val->get() = val;
        }
        else {
            t_vars.emplace(name, val);
        }
    }
    
    if (it != end_it && (it + 1) != end_it) {
        return bifunc_set_modif(it, end_it, sub_env);
    }
    else {
        return val;
    }
}

Cell CoreEnvironment::bifunc_setq_modif(Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, CellEnvironment& sub_env)
{
    if (beg_it == end_it) return nil;
    if (!is_symbol_c(*beg_it)) throw "bifunc_setq error";
    const auto& name = (*beg_it).to_atom().to_symbol();
    
    auto it = beg_it + 1;
    auto val = (it == end_it) ? nil : eval_quote(*it, sub_env);

    if (t_vars.find(name) != end(t_vars)) {
        t_vars.erase(name);
        t_vars.emplace(name, val);
    }
    else {
        auto env_val = sub_env.get(name);
        if (env_val) {
            env_val->get() = val;
        }
        else {
            t_vars.emplace(name, val);
        }
    }

    if (it != end_it && (it + 1) != end_it ) {
        return bifunc_set_modif(it + 1, end_it, sub_env);
    }
    else {
        return val;
    }
}
