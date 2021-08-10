#include "Lexer.h"

#include "CoreData.h"
#include "Funcs.h"

#include <algorithm>
#include <unordered_set>
#include <sstream>
using namespace std;

Syntaxer::Syntaxer(SExprsFarm& farm, bool skip_comments, bool upcase_mode):
    t_farm(farm),
    t_skip_comments(skip_comments),
    t_upcase_mode(upcase_mode)
{
}

void Syntaxer::set_skip_comments_mode(bool mode)
{
    t_skip_comments = mode;
}

bool Syntaxer::skip_comments_mode() const
{
    return t_skip_comments;
}

void Syntaxer::set_upcase_mode(bool mode)
{
    t_upcase_mode = mode;
}

bool Syntaxer::upcase_mode() const
{
    return t_upcase_mode;
}

std::pair < read_reason, char> Syntaxer::read_char(CoreInputStreamInt& stream)
{
    EndlessStream s(stream);
    if (!s) return { read_reason::stream_fail, 0 };
    auto result = s.read();
    process_char(result);
    return { read_reason::success, result };
}

std::pair<read_reason, Cell>  Syntaxer::read_sexpr(CoreInputStreamInt& stream)
{
    if (!stream.ready()) {
        return { read_reason::empty_input,  t_farm.get().nil };
    }
    try {
        auto s = TokenStream(t_skip_comments, stream);
        return gen_cell(s);
    }
    catch (const throw_token_stream_empty&) {
        return { read_reason::empty_input,  t_farm.get().nil };
    }
}

std::pair<read_reason, std::vector<Cell>> Syntaxer::read_sexprs(CoreInputStreamInt& stream)
{
    try {
        std::vector<Cell> result;
        auto result_pair = read_sexpr(stream);
        while (result_pair.first != read_reason::empty_input) {
            if (result_pair.first != read_reason::success) {
                return { result_pair.first, {} };
            }
            result.push_back(result_pair.second);
            result_pair = read_sexpr(stream);
        }
        return { read_reason::success , move(result) };
    }
    catch (const char* str) {
        std::cout << str << endl;
        throw str;
    }
    catch (...) {
        std::cout << "wtf" << endl;
        throw;
    }
}

SExprStream Syntaxer::read_sexprs_stream(CoreInputStreamInt& stream)
{
    return SExprStream(*this, stream);
}

void Syntaxer::process_char(char& symbol)
{
    if (t_upcase_mode) symbol = char(std::toupper(symbol));
}

void Syntaxer::process_symbol(std::string& symbol)
{
    if (t_upcase_mode) std::transform(symbol.begin(), symbol.end(), symbol.begin(), std::toupper);
}



inline bool isdig(char c)
{
    return isdigit(static_cast<unsigned char>(c)) != 0;
}

inline bool isdig_its(const char* beg, const char* end)
{
    return (beg != end) && (isdig(*beg) && (((beg + 1) == end) || (((beg + 1) != end) && isdig_its((beg + 1), end))));
}

inline bool is_integer_number_its(const char* beg, const char* end)
{
    if (beg == end) return false;
    if((isdig(*beg)) && (((beg + 1) == end) || ((beg + 1) != end) && isdig_its((beg + 1), end))) return true;
    if((*beg == '-' || *beg == '+') && (((beg + 1) != end) && isdig_its((beg + 1), end))) return true;
    return false;
}

inline bool is_half_integer_num(const char* str) {
    const char* beg = str; const char* end = str + strlen(str);
    if (beg == end) return false;
    if (auto it = find(beg, end, '.');  it != end) {
        return is_integer_number_its(beg, it) && (it + 1 == end);
    }
    return false;
}

inline bool is_integer_number(const char* str)
{
    return is_integer_number_its(str, str + strlen(str));
}

bool is_real_number_its(const char* beg, const char* end)
{
    if (auto it = find(beg, end, '.');  it != end) {
        return is_integer_number_its(beg, it) && (is_integer_number_its(it + 1, end));
    }
    return false;
}

inline bool is_real_number(const char* str) {
    return is_real_number_its(str, str + strlen(str));
}


std::pair<read_reason, Cell> Syntaxer::gen_cell(TokenStream& tokens)
{
    auto [token, val] = tokens.read();
    return gen_cell(token, move(val), tokens);
}

std::pair<read_reason, Cell> Syntaxer::gen_cell(tokens token, std::string&& val, TokenStream& tokens)
{
    if (token == tokens::Quote) {
        auto  result_pair = gen_cell(tokens);
        if (result_pair.first != read_reason::success) return result_pair;
        return { 
            read_reason::success,
            t_farm.get().make_list_cell({ t_farm.get().make_symbol_cell("QUOTE"), move(result_pair.second) }),
        };
    }

    if (token == tokens::OpenBracket) {
        std::vector<Cell> lst;
        for (;;) {
            auto [next_token, next_val] = tokens.read();
            if (next_token == tokens::CloseBracket) break;
            if (next_token == tokens::Dot) {
                if(lst.empty()) return { read_reason::wrong_input, t_farm.get().nil };
                auto [s_pair_token, s_pair_val] = tokens.read();
                auto  s_pair = gen_cell(s_pair_token, move(s_pair_val), tokens);
                if (s_pair.first != read_reason::success) return s_pair;

                Cell a = cons(lst.back(), s_pair.second, t_farm.get());
                //cout << a << endl;
                for (auto it = next(rbegin(lst)); it != rend(lst); ++it) {
                    a = cons(*it, a, t_farm.get());
                    //cout << a << endl;
                }

                auto [close_token, close_val] = tokens.read();
                if (close_token != tokens::CloseBracket) return { read_reason::wrong_input, t_farm.get().nil };
                else return { read_reason::success, move(a) };
            }
            auto  result_pair = gen_cell(next_token, move(next_val), tokens);
            if (result_pair.first != read_reason::success) return result_pair;
            lst.push_back(move(result_pair.second));
        }
        return { read_reason::success, t_farm.get().make_list_cell(begin(lst), end(lst)) };
    }
    else if (token == tokens::QExpr) {
        return   { read_reason::success , t_farm.get().make_symbol_cell(move(val)) };
    }
    else if (token == tokens::Expr) {
        if (is_real_number(val.c_str())) {
            stringstream s(val + " ");
            double n;
            s >> n;
            if (s.good())
            {
                return { read_reason::success, t_farm.get().make_number_cell(n) };
            }
            else
            {
                val.erase(val.find('.'), val.npos);
                return { read_reason::success, t_farm.get().make_number_cell(BigInt(move(val))) };
            }
        }
        if (is_integer_number(val.c_str())) {
            if (val[0] == '+') val.erase(begin(val));
            return { read_reason::success, t_farm.get().make_number_cell(BigInt(move(val))) };
        }
        else if (is_half_integer_num(val.c_str())) {
            if (val[0] == '+') val.erase(begin(val));
            val.erase(val.find('.'), val.npos);
            return { read_reason::success,t_farm.get().make_number_cell(BigInt(move(val))) };
        }
        if (val.empty() || (isdig(val[0]))) {
            return { read_reason::wrong_input, t_farm.get().nil };
        }
        process_symbol(val);
        if (val == CoreData::nil_str) return { read_reason::success , t_farm.get().nil };
        return   { read_reason::success , t_farm.get().make_symbol_cell(move(val)) };
    }
    else {
        throw "unknown token";
    }
}






TokenStream::TokenStream(bool skip_comments, CoreInputStreamInt& stream) :
    t_base(stream),
    t_skip_comments(skip_comments),
    t_stream(stream)
{
}

std::pair<tokens, std::string> TokenStream::read()
{
    t_base.next();
    while (t_is_skip_symbol(t_base.get()) || t_is_ignore_symbol(t_base.get())) {
        if (!t_base) {
            throw throw_token_stream_empty{};
        }
        t_base.next();
    }

    string exp;
    while (t_base && !t_is_skip_symbol(t_base.get()) && !t_is_break_symbol(t_base.get())) {
        if (t_is_ignore_symbol(t_base.get())) {
            t_base.next();
            continue;
        }
        exp += t_base.get();
        if (t_base) {
            t_base.next();
        }
        else {
            break;
        }
    }
    if (t_base.get() == '.') {
        if (!exp.empty()) {
            auto next = t_check_next(true);
            if (
                next == check_next_reason::empty_stream
                ||
                next == check_next_reason::skip_s
                ||
                next == check_next_reason::break_s
            ) 
            {
                return { tokens::Expr, exp + '.' };
            }
            auto [token, result] = read();
            if (token != tokens::Expr) {
                throw "input_error";
            }
            else {
                exp += '.';
                exp += result;
                return { tokens::Expr, exp };
            }
        }
        else {
            auto next = t_check_next(true);
            if (next == check_next_reason::skip_s) {
                return { tokens::Dot, "" };
            }
            if (
                next == check_next_reason::empty_stream
                ||
                next == check_next_reason::break_s
                )
            {
                return { tokens::Expr, "." };
            }
            return read();
        }
    }
    else if (exp.empty()) {
        return t_read_break_symbol();
    }
    else {
        if(t_base) t_base.unread_last();
        return { tokens::Expr, exp };
    }
}

bool TokenStream::alive() const
{
    return bool(t_base);
}

bool TokenStream::ready() const
{
    return bool(t_base);
}

std::string TokenStream::t_read_expr_under_DoubleQuote() {
    string result;
    while (t_base.get() != '"') {
        if (t_base.get() == '\\') {
            t_base.next();
        }
        result += t_base.get();
        t_base.next();
    }
    return result;
}

std::string TokenStream::t_read_expr_under_VerticalLine() {
    string result;
    while (t_base.get() != '|') {
        if (t_base.get() == '\\') {
            t_base.next();
        }
        result += t_base.get();
        t_base.next();
    }
    return result;
}

std::pair<tokens, std::string> TokenStream::t_read_break_symbol()
{
    if (t_base.get() == '\'') {
        return { tokens::Quote, "" };
    }
    else if (t_base.get() == '(') {
        return { tokens::OpenBracket, "" };
    }
    else if (t_base.get() == ')') {
        return { tokens::CloseBracket, "" };
    }
    else if (t_base.get() == '"') {
        t_base.next();
        return { tokens::QExpr, t_read_expr_under_DoubleQuote() };
    }
    else if (t_base.get() == '|') {
        t_base.next();
        return { tokens::QExpr, t_read_expr_under_VerticalLine() };
    }
    else if (t_base.get() == ';') {
        if (t_skip_comments) {
            while (t_base.get() != '\n') {
                t_base.next();
            }
        }
        return read();
    }
    else if (t_base.get() == '.') {
        return { tokens::Dot, "" };
    }
    else {
        throw "TokenStream: internal error unknow break symbol";
    }
}

const std::unordered_set<char> skip_symbols = { ' ', '\t'};
const std::unordered_set<char> break_sybols = { '"', '(', ')', '\'', '|', ';', '.'};

bool TokenStream::t_is_break_symbol(char symbol) const {
    return break_sybols.find(symbol) != end(break_sybols);
}

bool TokenStream::t_is_skip_symbol(char symbol) const {
    if (symbol == '\n') {
        return t_stream.get_mode() != CoreData::stream_read_mode::new_string;
    }
    return skip_symbols.find(symbol) != end(skip_symbols);
}

bool TokenStream::t_is_ignore_symbol(char symbol) const
{
    if (symbol == '\n') {
        return t_stream.get_mode() == CoreData::stream_read_mode::new_string;
    }
    return false;
}

TokenStream::check_next_reason TokenStream::t_check_next(bool skip_ignorse)
{
    if (!t_base) return check_next_reason::empty_stream;
    t_base.next();
    char symbol = t_base.get();
    t_base.unread_last();
    if (t_is_skip_symbol(symbol)) return check_next_reason::skip_s;
    if (t_is_break_symbol(symbol)) return check_next_reason::break_s;
    if (t_is_ignore_symbol(symbol) && skip_ignorse) {
        while (t_base && t_is_ignore_symbol(symbol)) {
            t_base.next();
            symbol = t_base.get();
        }
        if(!t_base)return check_next_reason::empty_stream;
        t_base.unread_last();
        return t_check_next(skip_ignorse);
    }
    else if (t_is_ignore_symbol(symbol)) return check_next_reason::ignore_s;
    return check_next_reason::another_s;
}

EndlessStream::EndlessStream(CoreInputStreamInt& stream):
    t_base(stream),
    t_buffer(),
    t_view_buf(t_buffer),
    t_pos(0)
{

}

EndlessStream::~EndlessStream() {
    if (!t_view_buf.empty()) {
        t_base.erase_atlast(t_base.last_line().size() - 1 - (t_view_buf.size() - int(t_view_buf.find('\n') != t_view_buf.npos)));
        t_base.unread_last_line();
    }
}

char EndlessStream::read() {
    while (t_view_buf.empty()) {
        if (!t_base.ready()) throw throw_endless_stream_error{};

        t_buffer = t_base.get_line();
        t_buffer += "\n";
        t_view_buf = t_buffer;
    }
    char result = t_view_buf.front();
    t_view_buf.remove_prefix(1);
    ++t_pos;
    return result;
}

EndlessStream::operator bool()const {
    if (t_view_buf.empty()) return t_base.ready();
    return true;
}

void EndlessStream::unread_last() {
    if (t_pos == 0) return;
    auto size = t_view_buf.size();
    if (!t_view_buf.empty() && size != t_buffer.size()) {
        t_view_buf = t_buffer;
        t_view_buf.remove_prefix((t_buffer.size() - size) - 1);
    }
    else {
        t_base.erase_atlast(t_base.last_line().size() - 2);
        t_base.unread_last_line();
        t_buffer.clear();
        t_view_buf = t_buffer;
    }
    --t_pos;
}

EndlessStreamHelper::EndlessStreamHelper(CoreInputStreamInt& s) :
    t_base(s),
    t_readed(false)
{

}

char EndlessStreamHelper::get() {
    if (!t_readed) {
        throw "EndlessStreamHelper: unreaded get";
    }
    return t_now;
}

void EndlessStreamHelper::next() {
    if (!t_readed) t_readed = true;
    t_now = t_base.read();
}

void EndlessStreamHelper::unread_last() {
    t_base.unread_last();
    t_readed = false;
}

EndlessStreamHelper::operator bool() const {
    return bool(t_base);
}

SExprStream::SExprStream(Syntaxer& owner, CoreInputStreamInt& stream):
    t_owner(owner),
    t_base(owner.skip_comments_mode(), stream)
{

}

std::pair<read_reason, Cell> SExprStream::read()
{
    try {
        return t_owner.gen_cell(t_base);
    }
    catch (const throw_token_stream_empty&) {
        return { read_reason::empty_input,  t_owner.t_farm.get().nil };
    }
}

bool SExprStream::alive() const
{
    return t_base.alive();
}

bool SExprStream::ready() const
{
    return t_base.ready();
}
