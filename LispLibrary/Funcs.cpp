#include "Funcs.h"

#include "Number.h"
#include "CarCdrIterator.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

std::ostream& operator<<(std::ostream& s, const Cell& exp)
{
    s << to_string(exp);
    return s;
}

// convert given Cell to a Lisp-readable string
std::string to_string(const Cell& exp)
{
    if (is_list(exp)) {
        return to_string(to_list(exp));
    }
    else if (is_atom(exp)) {
        return to_string(to_atom(exp));
    }
    else {
        throw "to_string: unknown object";
    }
}

std::string to_string(const DotPair& lst)
{
    if (lst.empty()) return CoreData::nil_str;
    std::string s("(");
    CarCdrConstIteration iteration(lst);
    auto it = begin(iteration);
    for (; next(it) != end(iteration); ++it) {
        s += to_string(*it);
        s += ' ';
    }
    s += to_string(car(it.get_pair()));
    s += ' ';
    if (is_atom(cdr(it.get_pair()))) {
        s += ". ";
        s += to_string(cdr(it.get_pair()));
    }

    if (s[s.length() - 1] == ' ') {
        s.erase(s.length() - 1);
    }
    s += ')';
    return s;
}

std::string to_string(const Atom& exp)
{
    if (is_symbol(exp)) {
        return to_string(to_symbol(exp));
    }
    else if (is_number(exp)) {
        return to_string(to_number(exp));
    }
    else {
        throw "to_string: unknown object";
    }
}

std::string to_string(const Symbol& exp)
{
    return exp.to_string();
}

std::string to_string(const Number& exp)
{
    if (exp.is_integer()) {
        return exp.to_integer().to_string();
    }
    else if (exp.is_real()) {
        double result = exp.to_real();
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
