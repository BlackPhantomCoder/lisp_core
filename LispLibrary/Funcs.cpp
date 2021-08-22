#include "Funcs.h"

#include "CarCdrIterator.h"
#include "CoreData.h"
#include "CoreEnv.h"

#include <unordered_set>
#include <cctype>

using namespace std;

const unordered_set<char> read_updcase_special_tokens = {'\\', ';', '\"', ',', '\'', '`', '|' , ' ' };
const unordered_set<char> noread_updcase_special_tokens = { '\\', '\"', '|', ' ', ',' };

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
    return (val) ? farm.T() : farm.nil();
}

bool is_null(const Cell& c)
{
    if (is_list(c)) {
        return is_null_list(to_list(c));
    }
    else if (is_symbol(c)) {
        return is_null_symbol(to_symbol(c));
    }
    return false;
}

bool is_null_symbol(const Symbol& c)
{
    return c.t_data.is_nil_symbol();
}

bool is_null_symbol(const Cell& c)
{
    return is_symbol(c) && is_null_symbol(to_symbol(c));
}

bool is_lambda_symbol(const Symbol& arg, SExprsFarm& farm)
{
    return arg == to_symbol(farm.lambda_symbol());
}

bool is_nlambda_symbol(const Symbol& arg, SExprsFarm& farm)
{
    return arg == to_symbol(farm.nlambda_symbol());
}

bool is_alambda_symbol(const Symbol& arg, SExprsFarm& farm)
{
    return is_lambda_symbol(arg, farm) || is_nlambda_symbol(arg, farm);
}

bool is_implicit_cond(const Cell& arg, SExprsFarm& farm)
{
    if (!is_list(arg)) return false;
    if (is_null_list(to_list(arg))) return false;
    const auto& c = car(arg);
    if (!is_list(c)) return false;
    if (is_null_list(to_list(c))) return true;
    const auto& cc = car(c);
    if (!is_symbol(cc)) return true;
    if (is_alambda_symbol(to_symbol(cc), farm)) return false;
    return true;
}

bool is_alambda_form(const Cell& arg, SExprsFarm& farm)
{
    return is_list(arg) && !is_null(arg) && is_symbol(car(arg)) && is_alambda_symbol(to_symbol(car(arg)), farm);
}

bool is_lambda_form(const Cell& arg, SExprsFarm& farm)
{
    if (!is_list(arg) || is_null(arg)) return false;
    const auto& c = car(arg);
    return is_symbol(c) && is_lambda_symbol(to_symbol(c), farm);
}

bool is_nlambda_form(const Cell& arg, SExprsFarm& farm)
{
    return is_list(arg) && !is_null(arg) && is_symbol(car(arg)) && is_nlambda_symbol(to_symbol(car(arg)), farm);
}

Cell& car(Cell& dp) {
    if(is_atom(dp) || is_null_list(to_list(dp))){
        return dp;
    }
    return to_list(dp).t_first;
}

Cell& cdr(Cell& dp) {
    if (is_atom(dp) || is_null_list(to_list(dp))) {
        return dp;
    }
    return to_list(dp).t_second;
}

const Cell& car(const Cell& dp) {
    if (is_atom(dp) || is_null_list(to_list(dp))) {
        return dp;
    }
    return to_list(dp).t_first;
}

const Cell& cdr(const Cell& dp) {
    if (is_atom(dp) || is_null_list(to_list(dp))) {
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
    //need to refactoring
    // ??
    if (is_atom(f) || is_null(f)) return s;
    return cons(car(f), append(cdr(f), s, farm), farm);
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
    CarCdrConstIterator it;
    CarCdrConstIterator end_it;
    vector<Cell> result;
    bool last = false;
};

Cell tree_copy(const Cell& rh, SExprsFarm& farm)
{
    if (is_null_list(to_list(rh))) return farm.make_empty_list_cell();

    stack<tree_copy_frame> stack;
    stack.push(tree_copy_frame{});
    stack.top().it = begin(rh);
    stack.top().end_it = end(rh);

    for (;;) {
        auto& frame = stack.top();
        if (frame.it == frame.end_it) {
            if (!frame.last) frame.result.push_back(farm.nil());
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

        if (next(frame.it) == frame.end_it) {
            frame.result.push_back(car(frame.it.get_elem()));
            if (!is_null(cdr(frame.it.get_elem()))) {
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
            if (is_null_list(to_list(rh))) {
                frame.result.push_back(*stack.top().it);
                ++frame.it;
            }
            else {
                stack.push({});
                stack.top().it = begin(*frame.it);
                stack.top().end_it = end(*frame.it);
            }
        }
    }
}

std::optional<std::reference_wrapper<const macro>> is_macro_call(const Cell& m, CoreEnvironment& e)
{
    if (!(!is_list(m) || is_null_list(to_list(m)))) {
        auto& c = car(m);
        if (is_symbol(c)) {
            if (auto f_opt = e.t_funcs.find(to_symbol(c))) {
                if (auto l = get_if<macro>(&f_opt->get())) {
                    return { *l };
                }
            }
        }
    }
    return nullopt;
}
