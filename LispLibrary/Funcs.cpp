#include "Funcs.h"

#include <sstream>
using namespace std;


std::list<std::string> tokenize(const std::string& str)
{
    std::list<std::string> tokens;
    const char* s = str.c_str();

    long brackets = 0;
    while (*s) {
        while (*s == ' ')
            ++s;

        if (*s == '(') {
            tokens.push_back("(");
            ++brackets;
            ++s;
        }
        else if (*s == ')') {
            tokens.push_back(")");
            --brackets;
            ++s;
        }
        else {
            const char* t = s;
            while (*t && *t != ' ' && *t != '(' && *t != ')') {
                ++t;
            }
            tokens.push_back(std::string(s, t));
            s = t;
        }
    }
    if (brackets != 0) throw "invalid_argument";
    return tokens;
}

inline bool isdig(char c)
{
    return isdigit(static_cast<unsigned char>(c)) != 0;
}

inline bool is_integer_number_its(const char* beg, const char* end)
{
    return (beg != end) && (isdig(*beg) || (((beg + 1) != end) && (*beg == '-') && isdig(*(beg + 1))));
}

inline bool is_integer_number(const char* str)
{
    return is_integer_number_its(str, str + strlen(str));
}

bool is_real_number_its(const char* beg, const char* end)
{
    if (auto it = find(beg, end, '.');  it != end) {
        return is_integer_number_its(beg, it) && is_integer_number_its(it + 1, end);
    }
    return false;
}

inline bool is_real_number(const char* str) {
    return is_real_number_its(str, str + strlen(str));
}

// return the Lisp expression in the given tokens
Cell read_from(std::list<std::string>& tokens)
{
    std::string token(std::move(tokens.front()));
    tokens.pop_front();
    if (token == "(") {
        std::list<Cell> lst;
        while (tokens.front() != ")")
            lst.push_back(read_from(tokens));
        tokens.pop_front();
        return make_list(std::move(lst));
    }
    else {
        if (is_real_number(token.c_str())) {
            stringstream s(token + " 1.0");
            double n;
            s >> n;
            if (s.good())
            {
                return make_atom(Atom(n));
            }
            else
            {
                token.erase(token.find('.'), token.npos);
                return make_atom(Atom(BigInt(move(token))));
            }
        }
        if (is_integer_number(token.c_str())) {
            return make_atom(Atom(BigInt(move(token))));
        }
        return  make_atom(Atom(move(token)));
    }
        
}

// return the Lisp expression represented by the given string
Cell read(const std::string& s) {
    std::list<std::string> tokens(tokenize(s));
    return read_from(tokens);
}

// convert given Cell to a Lisp-readable string
std::string to_string(const Cell& exp)
{
    if (exp.is_list()) {
        const auto& lst = exp.to_list();
        if (lst.empty()) return "nil";
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
        return exp.to_atom().to_symbol();
    }
    else if(exp.to_atom().is_integer()){
        return exp.to_atom().to_integer().to_string();
    }
    else {
        return to_string(exp.to_atom().to_real());
    }
}

std::vector<std::string> expressions_from_file(const std::string& path)
{
    return vector<std::string>();
}
