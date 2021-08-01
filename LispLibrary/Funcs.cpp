#include "Funcs.h"

#include "Number.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

// convert given Cell to a Lisp-readable string
std::string to_string(const Cell& exp)
{
    if (exp.is_list()) {
        const auto& lst = exp.to_list();
        if (lst.empty()) return CoreData::nil_str;
        std::string s("(");
        for (auto e = std::begin(lst); e != std::end(lst); ++e) {
            s += to_string(*e);
            s += ' ';
        }

        if (s[s.length() - 1] == ' ') {
            s.erase(s.length() - 1);
        }
        s += ')';
        return s;
    }
    else if (exp.to_atom().is_symbol()) {
        return exp.to_atom().to_symbol().to_string();
    }
    else if (exp.to_atom().is_number()) {
        if (exp.to_atom().to_number().is_integer()) {
            return exp.to_atom().to_number().to_integer().to_string();
        }
        else if(exp.to_atom().to_number().is_real()) {
            double result = exp.to_atom().to_number().to_real();
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
            throw "error number cell";
        }
    }
    else {
        throw "error cell";
    }
}

bool is_null(const Cell& c, SymbolsProvider& provider) {
    if (is_symbol(c) && to_symbol(c) == provider.nil_symbol) return true;
    if (!c.is_list()) return false;
    return c.to_list().empty();
}

Cell bool_cast(bool val, SymbolsProvider& provider) {
    if (val) return provider.T;
    else return provider.nil;
}

bool is_T(const Cell& c, SymbolsProvider& provider) {
    if (!is_symbol(c)) return false;
    return to_symbol(c) == provider.T_symbol;
}

bool is_implicit_cond(const Cell& arg, SymbolsProvider& provider)
{
    if (!arg.is_list()) return false;
    if (arg.to_list().empty()) return false;
    if (!arg.to_list()[0].is_list()) return false;
    if (
            !arg.to_list()[0].to_list().empty()
            &&
            is_symbol(arg.to_list()[0].to_list()[0])
            &&
            (
                to_symbol(arg.to_list()[0].to_list()[0]) == provider.lambda_symbol
                ||
                to_symbol(arg.to_list()[0].to_list()[0]) == provider.nlambda_symbol
            )
        )
    {
        return false;
    }
    return true;
}
