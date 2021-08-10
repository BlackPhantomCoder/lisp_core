#include "Funcs.h"

#include "CarCdrIterator.h"
#include "CoreData.h"
#include "LambdaCell.h"
#include "CoreEnv.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

std::string to_string_list(const Cell& exp, SExprsFarm& farm);
std::string to_string_symbol(const Cell& exp);
std::string to_string_number(const Cell& exp);

std::string to_string(const lambda& fnc, SExprsFarm& farm)
{
    if (is_lambda(fnc)) {
        string str = "(";
        str += CoreData::lambda_str;

        str += " ";
        if (is_spread(fnc)) {
            str += to_string(fnc.params, farm);
        }
        else if (is_nospread(fnc)) {
            str += to_symbol(car(fnc.params)).to_string();
        }
        else {
            throw "to_string: unknown spread type";
        }
        str += " ";


        if (!is_null(fnc.body, farm)) {
            str += to_string(car(fnc.body), farm);
        }
        else {
            str += to_string(fnc.body, farm);
        }
        str += ")";
        return str;
    }
    else if (is_nlambda(fnc)) {
        string str = "(";
        str += CoreData::nlambda_str;

        str += " ";
        if (is_spread(fnc)) {
            str += to_string(car(fnc.body), farm);
        }
        else  if (is_nospread(fnc)) {
            str += to_symbol(car(fnc.params)).to_string();
        }
        else {
            throw "to_string: unknown spread type";
        }
        str += " ";

        if (!is_null(fnc.body, farm)) {
            str += to_string(car(fnc.body), farm);
        }
        else {
            str += to_string(fnc.body, farm);
        }
        str += ")";
        return str;
    }
    else {
        throw "to_string ...?";
    }
}

// convert given Cell to a Lisp-readable string
std::string to_string(const Cell& exp, SExprsFarm& farm)
{
    if (is_list(exp)) {
        return to_string_list(exp, farm);
    }
    else if (is_symbol(exp)) {
        return to_string_symbol(exp);
    }
    else if (is_number(exp)) {
        return to_string_number(exp);
    }
    else {
        throw "to_string: unknown object";
    }
}

Cell bool_cast(bool val, SExprsFarm& farm)
{
    return (val) ? farm.T : farm.nil;
}

bool is_null(const Cell& c, SExprsFarm& farm)
{
    if (is_list(c)) {
        return is_null(to_list(c));
    }
    else if (is_symbol(c)) {
        return to_symbol(c) == to_symbol(farm.nil);
    }
    return false;
}

bool is_null_symbol(const Symbol& c, SExprsFarm& farm)
{
    return c == to_symbol(farm.nil);
}

bool is_null_symbol(const Cell& c, SExprsFarm& farm)
{
    return is_symbol(c) && to_symbol(c) == to_symbol(farm.nil);
}

bool is_lambda_symbol(const Symbol& arg, SExprsFarm& farm)
{
    return arg == to_symbol(farm.lambda_symbol);
}

bool is_nlambda_symbol(const Symbol& arg, SExprsFarm& farm)
{
    return arg == to_symbol(farm.nlambda_symbol);
}

bool is_alambda_symbol(const Symbol& arg, SExprsFarm& farm)
{
    return is_lambda_symbol(arg, farm) || is_nlambda_symbol(arg, farm);
}




bool is_implicit_cond(const Cell& arg, SExprsFarm& farm)
{
    if (!is_list(arg)) return false;
    if (is_null(arg, farm)) return false;
    if (!is_list(car(arg))) return false;
    if (is_null(car(arg), farm)) return true;
    if (!is_symbol(car(car(arg)))) return true;
    if (is_alambda_symbol(to_symbol(car(car(arg))), farm)) return false;
    return true;
}

bool is_alambda_form(const Cell& arg, SExprsFarm& farm)
{
    return is_list(arg) && !is_null(arg, farm) && is_symbol(car(arg)) && is_alambda_symbol(to_symbol(car(arg)), farm);
}

bool is_lambda_form(const Cell& arg, SExprsFarm& farm)
{
    return is_list(arg) && !is_null(arg, farm) && is_symbol(car(arg)) && is_lambda_symbol(to_symbol(car(arg)), farm);
}

bool is_nlambda_form(const Cell& arg, SExprsFarm& farm)
{
    return is_list(arg) && !is_null(arg, farm) && is_symbol(car(arg)) && is_nlambda_symbol(to_symbol(car(arg)), farm);
}

std::string to_string_list(const Cell& lst, SExprsFarm& farm)
{
    if (is_null(lst, farm)) return CoreData::nil_str;
    std::string s("(");
    CarCdrConstIteration iteration(lst, farm);
    auto it = begin(iteration);
    for (; next(it) != end(iteration); ++it) {
        s += to_string(*it, farm);
        s += ' ';
    }
    s += to_string(car(it.get_elem()), farm);
    s += ' ';
    if (!is_null(cdr(it.get_elem()), farm)) {
        s += ". ";
        s += to_string(cdr(it.get_elem()), farm);
    }

    if (s[s.length() - 1] == ' ') {
        s.erase(s.length() - 1);
    }
    s += ')';
    return s;
}


std::string to_string_symbol(const Cell& exp)
{
    return to_symbol(exp).to_string();
}

std::string to_string_number(const Cell& exp)
{
    const auto& number = to_number(exp);
    if (is_integer(number)) {
        return to_integer(number).to_string();
    }
    else if (is_real(number)) {
        double result = to_real(number);
        double a = 0;
        double b = modf(result, &a);
        unsigned i = 0;

        while (a >= 1) {
            a /= 10;
            ++i;
            if (i == Number::epsilon - 1) break;
        }

        while (b > 0) {
            b *= 10;
            b = modf(b, &a);
            ++i;
            if (i == Number::epsilon - 1) break;
        }

        ostringstream s;
        s << setprecision(i + 1);
        s << result;
        return s.str();
    }
    else {
        throw "to_string: unknown object";
    }
}

Cell& car(Cell& dp) {
    if(is_atom(dp) || is_null(to_list(dp))){
        return dp;
    }
    return to_list(dp).t_first;
}

Cell& cdr(Cell& dp) {
    if (is_atom(dp) || is_null(to_list(dp))) {
        return dp;
    }
    return to_list(dp).t_second;
}

const Cell& car(const Cell& dp) {
    if (is_atom(dp) || is_null(to_list(dp))) {
        return dp;
    }
    return to_list(dp).t_first;
}

const Cell& cdr(const Cell& dp) {
    if (is_atom(dp) || is_null(to_list(dp))) {
        return dp;
    }
    return to_list(dp).t_second;
}

Cell cons(const Cell& f, const Cell& s, SExprsFarm& farm)
{
    return farm.make_list_cell(f, s);
}

Cell append(const Cell& f, const Cell& s, SExprsFarm& farm)
{

    // ??
    if (is_atom(f) || is_null(f, farm)) return s;
    return cons(car(f), append(cdr(f), s, farm), farm);

    //if (is_atom(f) || is_null(to_list(f))) return s;
    //std::cout << f << std::endl;
    //std::cout << s << std::endl;
    //CarCdrConstIteration interation1(to_list(f));
    //Cell result = make_list_cell(std::begin(interation1), std::end(interation1), provider);
    //if (is_atom(s)) {
    //    CarCdrConstIteration interation2(to_list(s));
    //    for (const auto& cell : interation2) {
    //        result = cons_cell(cell, result, provider);
    //    }
    //}
    //else {
    //    result = cons_cell(s, result, provider);
    //}
    //return result;
}

void rplaca(DotPair& rh, const Cell& exp)
{
    rh.t_first = exp;
}

void rplacd(DotPair& rh, const Cell& exp)
{
    rh.t_second = exp;
}

struct tree_copy_frame {
    CarCdrConstIteration iteration;
    CarCdrConstIterator it;

    vector<Cell> result;
    bool last = false;
};

Cell tree_copy(const Cell& rh, SExprsFarm& farm)
{
    if(is_null(to_list(rh))) return farm.nil;

    stack<tree_copy_frame> stack;
    stack.push(tree_copy_frame{ {rh, farm}, CarCdrConstIterator{farm}, {} });
    stack.top().it = begin(stack.top().iteration);

    for(;;) {
        auto& frame = stack.top();
        if (frame.it == end(frame.iteration)) {
            if(!frame.last)frame.result.push_back(farm.nil);
            if (stack.size() == 1) {
                return cons_range(begin(frame.result), end(frame.result), farm);
            }
            else {
                auto lst = cons_range(begin(frame.result), end(frame.result), farm);
                stack.pop();
                stack.top().result.push_back(lst);
                ++stack.top().it;
            }
            continue;
        }

        if (next(frame.it) == end(frame.iteration)) {
            frame.result.push_back(car(frame.it.get_elem()));
            if(!is_null(cdr(frame.it.get_elem()), farm)) {
                frame.result.push_back(cdr(frame.it.get_elem()));
                frame.last = true;
            }
            ++frame.it;
            continue;
        }

        if (is_atom(*frame.it)) {
            frame.result.push_back(*stack.top().it);
            ++frame.it;
        }
        else {
            if (is_null(to_list(rh))) {
                frame.result.push_back(*stack.top().it);
                ++frame.it;
            }
            else {
                stack.push({ {*frame.it, farm}, CarCdrConstIterator{farm}, {} });
                stack.top().it = begin(stack.top().iteration);
            }
        }
    }
}
