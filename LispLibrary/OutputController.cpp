#include "OutputController.h"
#include "LambdaCell.h"
#include "Funcs.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_set>

using namespace std;

OutputController::OutputController(SExprsFarm& farm):
    t_farm(farm)
{
}


//need refactoring (recursion)
std::string OutputController::to_string(const Cell& exp)
{
    if (is_list(exp)) {
        return t_to_string_list(exp);
    }
    else if (is_symbol(exp)) {
        return to_string(to_symbol(exp));
    }
    else if (is_number(exp)) {
        return to_string(to_number(exp));
    }
    else {
        throw "to_string: unknown object";
    }
}

std::string OutputController::to_string_raw(const Cell& exp)
{
    if (is_list(exp)) {
        return t_to_string_list_raw(exp);
    }
    else if (is_symbol(exp)) {
        return to_string_raw(to_symbol(exp));
    }
    else if (is_number(exp)) {
        return to_string_raw(to_number(exp));
    }
    else {
        throw "to_string: unknown object";
    }
}

void OutputController::set_read_upcase(bool val)
{
    t_read_upcase = val;
}

std::string OutputController::t_to_string_list(const Cell& d)
{
    if (is_null_list(to_list(d))) return CoreData::nil_str;
    std::string s("(");

    auto it = begin(d);
    for (; next(it) != end(d); ++it) {
        s += to_string(*it);
        s += ' ';
    }
    s += to_string(car(it.get_elem()));
    s += ' ';
    if (!is_null(cdr(it.get_elem()))) {
        s += ". ";
        s += to_string(cdr(it.get_elem()));
    }

    if (s[s.length() - 1] == ' ') {
        s.erase(s.length() - 1);
    }
    s += ')';
    return s;
}

const std::unordered_set<char> skip_symbols = { ' ', '\t' };
const std::unordered_set<char> specital_single_symbols = { '.', ',', '|', '\\', '\"', '\'', '`', '(', ')' };

std::string OutputController::to_string(const Symbol& s)
{
    std::string str = s.to_string();
    if (str.empty()) return "||";
    if (str.size() == 1) {
        if (skip_symbols.find(str[0]) != end(skip_symbols)) return string("|") + str[0] + "|";
        if(specital_single_symbols.find(str[0]) != end(specital_single_symbols) || isdigit((unsigned char)str[0])) return  string("\\") + str[0];
    }
    
    if (isdigit((unsigned char)str[0])) {
        str.insert(begin(str), '|');
        str.push_back('|');
        return str;
    }
    bool have_special = 
        find_if(begin(str), end(str), [ruc = t_read_upcase](char c) {return is_special_symbol(ruc, c); }) != end(str);
    if (have_special) {
        if (str.size() == 1) {
            str.insert(begin(str), '\\');
            return str;
        }
        else {

            if (auto pos = str.find('\\'); pos != str.npos) {
                while (pos != str.npos) {
                    str.insert(pos, 1, '\\');
                    pos = str.find('\\', pos + 2);
                }
            }
            if (auto pos = str.find('|'); pos != str.npos) {
                while (pos != str.npos) {
                    str.insert(pos, 1, '\\');
                    pos = str.find('|', pos + 2);
                }
            }
            str.insert(begin(str), '|');
            str.push_back('|');
          
            return str;
        }
    }
    return str;
}

std::string OutputController::to_string(const Number& n)
{
    if (is_integer(n)) {
        return to_integer(n).to_string();
    }
    else if (is_real(n)) {
        double result = to_real(n);
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
        auto str = s.str();
        if (auto pos = str.find(','); pos != str.npos) {
            str[pos] = '.';
        }
        return str;
    }
    else {
        throw "to_string: unknown object";
    }
}

std::string OutputController::t_to_string_list_raw(const Cell& d)
{
    if (is_null_list(to_list(d))) return CoreData::nil_str;
    std::string s("(");

    auto it = begin(d);
    for (; next(it) != end(d); ++it) {
        s += to_string_raw(*it);
        s += ' ';
    }
    s += to_string_raw(car(it.get_elem()));
    s += ' ';
    if (!is_null(cdr(it.get_elem()))) {
        s += ". ";
        s += to_string_raw(cdr(it.get_elem()));
    }

    if (s[s.length() - 1] == ' ') {
        s.erase(s.length() - 1);
    }
    s += ')';
    return s;
}

std::string OutputController::to_string_raw(const Symbol& s)
{
    return s.to_string();
}

std::string OutputController::to_string_raw(const Number& n)
{
    if (is_integer(n)) {
        return to_integer(n).to_string();
    }
    else if (is_real(n)) {
        ostringstream s;
        s << to_real(n);
        auto str = s.str();
        if (auto pos = str.find(','); pos != str.npos) {
            str[pos] = '.';
        }
        return str;
    }
    else {
        throw "to_string: unknown object";
    }
}
