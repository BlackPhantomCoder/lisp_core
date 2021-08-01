#include "Lexer.h"
#include <string>
#include <list>
#include <algorithm>
#include <unordered_set>
#include "CoreData.h"
#include "Funcs.h"
#include <sstream>
using namespace std;

Syntaxer::Syntaxer(Symbols& symbols, bool skip_comments, bool upcase_mode):
    t_symbols(symbols),
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
        return { read_reason::empty_input,  make_symbol_cell(CoreData::nil_str, t_symbols) };
    }
    try {
        EndlessStream es(stream);
        auto s = TokenStream(t_skip_comments, es);
        return gen_cell(s);
    }
    catch (const throw_token_stream_empty&) {
        return { read_reason::empty_input,  make_symbol_cell(CoreData::nil_str, t_symbols) };
    }
}

std::pair<read_reason, std::vector<Cell>> Syntaxer::read_sexprs(CoreInputStreamInt& stream)
{
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

void Syntaxer::process_char(char& symbol)
{
    if (t_upcase_mode) symbol = std::toupper(symbol);
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
    return
        (
            (beg != end)
            &&
            ((isdig(*beg)) && (((beg + 1) == end) || ((beg + 1) != end) && isdig_its((beg + 1), end)))
            ||
            ((*beg == '-' || *beg == '+') && (((beg + 1) != end) && isdig_its((beg + 1), end)))
        );
             
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
        return { read_reason::success, make_list({ make_symbol_cell("QUOTE", t_symbols), move(result_pair.second) }) };
    }

    if (token == tokens::OpenBracket) {
        std::list<Cell> lst;
        for (;;) {
            auto [next_token, next_val] = tokens.read();
            if (next_token == tokens::CloseBracket) break;
            auto  result_pair = gen_cell(next_token, move(next_val), tokens);
            if (result_pair.first != read_reason::success) return result_pair;
            lst.push_back(move(result_pair.second));
        }
        return { read_reason::success, make_list(std::move(lst)) };
    }
    else if (token == tokens::QExpr) {
        return   { read_reason::success , make_symbol_cell(move(val), t_symbols) };
    }
    else if (token == tokens::Expr) {
        if (is_real_number(val.c_str())) {
            stringstream s(val + " ");
            double n;
            s >> n;
            if (s.good())
            {
                return { read_reason::success, make_number(Number(n)) };
            }
            else
            {
                val.erase(val.find('.'), val.npos);
                return { read_reason::success, make_number(Number(BigInt(move(val)))) };
            }
        }
        if (is_integer_number(val.c_str())) {
            if (val[0] == '+') val.erase(begin(val));
            return { read_reason::success,make_number(Number(BigInt(move(val)))) };
        }
        if (val.empty() || (isdig(val[0]))) {
            return { read_reason::wrong_input, make_symbol_cell(CoreData::nil_str, t_symbols) };
        }
        process_symbol(val);
        if (val == CoreData::nil_str) return { read_reason::success , make_list({}) };
        return   { read_reason::success , make_symbol_cell(move(val), t_symbols) };
    }
    else {
        throw "unknown token";
    }
}






TokenStream::TokenStream(bool skip_comments, EndlessStream& stream) :
    t_base(stream),
    t_skip_comments(skip_comments)
{
}

std::pair<tokens, std::string> TokenStream::read()
{
    EndlessStreamHelper s(t_base);
    return t_read(s);
}

std::pair<tokens, std::string> TokenStream::t_read(EndlessStreamHelper& s)
{
    while (is_skip_symbol(s.get())) {
        if (!s) {
            throw throw_token_stream_empty{};
        }
        s.next();
    }
    
    string exp;
    while (!is_skip_symbol(s.get()) && !is_break_symbol(s.get())) {
        exp += s.get();
        if (s) {
            s.next();
        }
        else {
            break;
        }
    }
    s.unread_last();
    if (exp.empty()) {
        return read_break_symbol(s);
    }
    else {
        return { tokens::Expr, exp };
    }
}

std::string TokenStream::read_expr_under_DoubleQuote(EndlessStreamHelper& s) {
    string result;
    while (s.get() != '"') {
        if (s.get() == '\\') {
            s.next();
        }
        result += s.get();
        s.next();
    }
    s.next();
    return result;
}

std::string TokenStream::read_expr_under_VerticalLine(EndlessStreamHelper& s) {
    string result;
    while (s.get() != '|') {
        if (s.get() == '\\') {
            s.next();
        }
        result += s.get();
        s.next();
    }
    s.next();
    return result;
}

std::pair<tokens, std::string> TokenStream::read_break_symbol(EndlessStreamHelper& s)
{
    if (s.get() == '\'') {
        return { tokens::Quote, "" };
    }
    else if (s.get() == '(') {
        return { tokens::OpenBracket, "" };
    }
    else if (s.get() == ')') {
        return { tokens::CloseBracket, "" };
    }
    else if (s.get() == '"') {
        s.next();
        return { tokens::QExpr, read_expr_under_DoubleQuote(s) };
    }
    else if (s.get() == '|') {
        s.next();
        return { tokens::QExpr,read_expr_under_VerticalLine(s) };
    }
    else if (s.get() == ';') {
        if (t_skip_comments) {
            while (s.get() != '\n') {
                s.next();
            }
        }
        return read();
    }
    else {
        throw "TokenStream: internal error unknow break symbol";
    }
}

const std::unordered_set<char> skip_symbols = { ' ', '\t', '\n' };
const std::unordered_set<char> break_sybols = { '"', '(', ')', '\'', '|', ';'};

bool TokenStream::is_break_symbol(char symbol) const {
    return break_sybols.find(symbol) != end(break_sybols);
}

bool TokenStream::is_skip_symbol(char symbol) const {
    return skip_symbols.find(symbol) != end(skip_symbols);
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
        t_base.unread_some(t_view_buf.size() - int(t_view_buf.find('\n') != t_view_buf.npos));
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
        t_base.unread_some(1);
        t_buffer.clear();
        t_view_buf = t_buffer;
    }
}

EndlessStreamHelper::EndlessStreamHelper(EndlessStream& s) :
    t_base(s)
{

}

char EndlessStreamHelper::get() {
    if (!t_readed) {
        t_now = t_base.read();
        t_readed = true;
    }
    return t_now;
}

void EndlessStreamHelper::next() {
    t_now = t_base.read();
}

void EndlessStreamHelper::unread_last() {
    t_base.unread_last();
    t_readed = false;
}

EndlessStreamHelper::operator bool() const {
    return bool(t_base);
}