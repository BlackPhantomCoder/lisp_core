#include "Funcs.h"

#include "Number.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

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
Cell read_from(std::list<std::string>& tokens, const IPublicCoreEnvironmentProvider& env)
{
    std::string token(std::move(tokens.front()));
    tokens.pop_front();

    if (token == "'") {
        auto cell = read_from(tokens, env);
        return make_list({ make_atom(Atom("QUOTE")), move(cell) });
    }

    if (token == "(") {
        std::list<Cell> lst;
        while (tokens.front() != ")")
            lst.push_back(read_from(tokens, env));
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
        const auto& vars = env.get().vars();

        if (auto it = vars.find(CoreEnvironment::read_up_case_str);
            it == end(vars)
            ||
            to_string(it->second) != CoreEnvironment::nil_str
           ) 
            {
                std::transform(token.begin(), token.end(), token.begin(), std::toupper);
            }
        return  make_atom(Atom(move(token)));
    }
        
}

// convert given Cell to a Lisp-readable string
std::string to_string(const Cell& exp)
{
    if (exp.is_list()) {
        const auto& lst = exp.to_list();
        if (lst.empty()) return CoreEnvironment::nil_str;
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
    while ((pos = str.find(symbol)) != str.npos) {
        str.erase(pos);
    }
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


std::pair<bool, Cell> read_one_expression_from_string(
    const std::string& str,
    const IPublicCoreEnvironmentProvider& env,
    bool skip_comments
)
{
    istringstream s(str);
    return read_one_expression_from_stream(s, env, stream_read_mode::new_string, skip_comments);
}

std::pair<bool, Cell> read_one_expression_from_stream(
    std::istream& in,
    const IPublicCoreEnvironmentProvider& env,
    stream_read_mode mode,
    bool skip_comments
)
{
    std::list<std::string> tokens;

    long brackets = 0;
    auto pos = in.tellg();

    bool finish = false;
    while (in && !finish) {
        string str = get_prep_line(in, skip_comments);
        const char* s = str.c_str();
        while (*s) {
            while (*s == ' ' || *s == '\t')
                ++s;

            if (*s == '\'') {
                tokens.push_back("'");
                ++s;
                continue;
            }
            else if (*s == '(') {
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
                if (!*t && in && mode == stream_read_mode::s_expression) {
                    str = string(s) + get_prep_line(in, skip_comments);
                    s = str.c_str();
                    continue;
                }
                tokens.push_back(std::string(s, t));
                delete_symbol(tokens.back(), '\t');
                s = t;
            }
            if (brackets == 0) {
                auto readed = str.size() - strlen(s);
                pos += readed;
                if (readed != str.size()) {
                    in.seekg(pos);
                    in.clear();
                }
                finish = true;
                break;
            }
        }
    }
    if (brackets != 0) {
        throw "invalid_argument";
    }
    else if(tokens.empty()){
        return { false, Cell() };
    }
    else {
        return { true, read_from(tokens, env) };
    }
}

std::vector<Cell> read_expressions_from_string(
    const std::string& str,
    const IPublicCoreEnvironmentProvider& env,
    bool skip_comments)
{
    istringstream s(str);
    return read_expressions_from_stream(s, env, stream_read_mode::s_expression, skip_comments);
}

std::vector<Cell> read_expressions_from_stream(
    std::istream& in,
    const IPublicCoreEnvironmentProvider& env,
    stream_read_mode mode,
    bool skip_comments
)
{
    std::vector<Cell> result;
    while (in) {
        auto [result_reason, cell] = read_one_expression_from_stream(in, env, mode, skip_comments);
        if (result_reason) {
            result.push_back(move(cell));
        }
    }
    return result;
}

Cell bool_cast(bool val) {
    if (val)  return CoreEnvironment::T;
    return CoreEnvironment::nil;
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
    return c.to_atom().is_number() && c.to_atom().to_number().is_integer();
}

bool is_real_number_c(const Cell& c) {
    if (!c.is_atom()) return false;
    return c.to_atom().is_number() && c.to_atom().to_number().is_real();
}

bool is_number2_c(const Cell& c) {
    if (!c.is_atom()) return false;
    return c.to_atom().is_number();
}

bool is_T(const Cell& c) {
    if (!c.is_atom()) return false;
    if (!is_symbol_c(c)) return false;
    return c.to_atom().to_symbol() == CoreEnvironment::T_str;
}

bool is_implicit_cond(const Cell& arg)
{
    if (!arg.is_list()) return false;
    if (!arg.to_list()[0].is_list()) return false;
    if (
        !arg.to_list()[0].to_list().empty()
        &&
        arg.to_list()[0].to_list()[0].is_atom()
        &&
        arg.to_list()[0].to_list()[0].to_atom().is_symbol()
        &&
        (
            arg.to_list()[0].to_list()[0].to_atom().to_symbol() == CoreEnvironment::lambda_str
            ||
            arg.to_list()[0].to_list()[0].to_atom().to_symbol() == CoreEnvironment::nlambda_str
        )
        )
    {
        return false;
    }
    return true;
}
