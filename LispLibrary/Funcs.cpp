#include "Funcs.h"

#include "Number.h"

#include <sstream>
#include <iomanip>
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
        else if (*s == '"') {
            auto s2 = s;
            ++s2;
            while (*s2 != '"') {
                ++s2;
                if (*s2 == '\0') throw "invalid input";
            }
            ++s2;
            tokens.push_back(std::string(s, s2));
            s = s2;
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
            stringstream s(token + " ");
            double n;
            s >> n;
            if (s.good())
            {
                return make_atom(Atom(Number(n)));
            }
            else
            {
                token.erase(token.find('.'), token.npos);
                return make_atom(Atom(Number(BigInt(move(token)))));
            }
        }
        if (is_integer_number(token.c_str())) {
            return make_atom(Atom(Number(BigInt(move(token)))));
        }
        return  make_atom(Atom(move(token)));
    }
        
}

// return the Lisp expression represented by the given string
Cell read(const std::string& s) {
    if (s.empty()) return make_atom(Atom("nil"));
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

void delete_symbol(std::string& str, char symbol) {
    size_t pos;
    while ( (pos = str.find(symbol)) != str.npos) {
        str.erase(pos);
    }
}

std::vector<Cell> listp_expressions_from_string(const std::string& str)
{
    std::vector<Cell> result;
    std::list<std::string> tokens;
    const char* s = str.c_str();

    long brackets = 0;
    while (*s) {
        while (*s == ' ' || *s == '\n' || *s == '\t')
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
        else if (*s == '"') {
            auto s2 = s;
            ++s2;
            while (*s2 != '"') {
                ++s2;
                if (*s2 == '\0') throw "invalid input";
            }
            ++s2;
            tokens.push_back(std::string(s, s2));
            s = s2;
        }
        else {
            const char* t = s;
            while (*t && *t != ' ' && *t != '(' && *t != ')') {
                ++t;
            }
            tokens.push_back(std::string(s, t));
            delete_symbol(tokens.back(), '\n');
            delete_symbol(tokens.back(), '\t');
            s = t;
        }
        if (brackets == 0) {
            result.push_back(read_from(tokens));
        }
    }
    if (brackets != 0) throw "invalid_argument";
    return result;
}

string get_prep_line(std::istream& in, bool skip_comments) {
    string str;
    getline(in, str);
    if (skip_comments) {
        if (auto pos = str.find(';'); pos != str.npos) {
            str.erase(pos, str.size());
        }
    }
    return str;
}

std::vector<Cell> listp_expressions_from_stream(std::istream& in, bool skip_comments)
{
    std::vector<Cell> result;
    std::list<std::string> tokens;

    long brackets = 0;
    while (in) {
        string str = get_prep_line(in, skip_comments);
        const char* s = str.c_str();
        while (*s) {
            while (*s == ' ' || *s == '\n' || *s == '\t')
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
            else if (*s == '"') {
                auto s2 = s;
                ++s2;
                while (*s2 && *s2 != '"') {
                    ++s2;
                }
                if (!*s2 && in) {
                    str = string(s) + get_prep_line(in, skip_comments);
                    s = str.c_str();
                    continue;
                }
                ++s2;
                tokens.push_back(std::string(s, s2));
                s = s2;
            }
            else {
                const char* t = s;
                while (*t && *t != ' ' && *t != '(' && *t != ')') {
                    ++t;
                }
                if (!*t && in) {
                    str = string(s) + get_prep_line(in, skip_comments);
                    s = str.c_str();
                    continue;
                }
                tokens.push_back(std::string(s, t));
                delete_symbol(tokens.back(), '\n');
                delete_symbol(tokens.back(), '\t');
                s = t;
            }
            if (brackets == 0) {
                result.push_back(read_from(tokens));
            }
        }
    }
    if (brackets != 0) throw "invalid_argument";
    return result;
}