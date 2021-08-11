#include "Funcs.h"

#include "CarCdrIterator.h"
#include "CoreData.h"
#include "CoreEnv.h"

#include <unordered_set>
#include <cctype>

using namespace std;

const unordered_set<char> read_updcase_special_tokens = {'\\', ';', '\"', ',', '\'', '`', '|' , ' ' };
const unordered_set<char> noread_updcase_special_tokens = { '\\', '\"', '|', ' ' };

bool is_special_symbol(bool read_upcase, unsigned char token) {
    if (read_upcase) {
        if (auto it = read_updcase_special_tokens.find(token); it != end(read_updcase_special_tokens)) {
            return true;
        }
        if (islower(token)) {
            return true;
        }
    }
    else {
        if (auto it = noread_updcase_special_tokens.find(token); it != end(noread_updcase_special_tokens)) {
            return true;
        }
    }
    return false;
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