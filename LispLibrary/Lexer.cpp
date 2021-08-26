#include "Lexer.h"

#include "CoreData.h"
#include "Funcs.h"
#include "CoreEnv.h"

#include <algorithm>
#include <unordered_set>
#include <sstream>
using namespace std;

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

const std::unordered_set<char> skip_symbols = { '\r', ' ', '\t'};
const std::unordered_set<char> break_sybols = { '"', '(', ')', '\'', '|', ';', '.', ','};

Cell macrochar_double_quote(read_helper& h) {
    std::string result;
    auto& stream = h.get_stream();
    while (stream.alive()) {
        auto ch = stream.get_char();
        if (ch == '\\') {
            if (!stream.alive()) throw throw_token_stream_empty{};
            result += stream.get_char();
            continue;
        }
        if (ch == '"') {
            return h.get_farm().make_symbol_cell(move(result));
        }
        result += ch;
    }
    throw throw_token_stream_empty{};
}

Cell macrochar_vertical_line(read_helper& h) {
    std::string result;
    auto& stream = h.get_stream();
    while (stream.alive()) {
        auto ch = stream.get_char();
        if (ch == '\\') {
            if (!stream.alive()) throw throw_token_stream_empty{};
            result += stream.get_char();
            continue;
        }
        if (ch == '|') {
            return h.get_farm().make_symbol_cell(move(result));
        }
        result += ch;
    }
    throw throw_token_stream_empty{};
}

Cell macrochar_left_round_braket(read_helper& h) {
    bool pair = false;
    vector<Cell, CoreData::allocator<Cell>> result;
    auto& stream = h.get_stream();
    while (stream.alive()) {
        h.skip_comments_and_spaces_if();
        auto ch = stream.peek_char();
        if (ch == ')') {
            stream.get_char();
            if(pair) return cons_range(begin(result), end(result), h.get_farm());
            return h.get_farm().make_list_cell(begin(result), end(result));
        }

        if (ch == '.') {
            stream.get_char();
            if (h.is_space_char(stream.peek_char()) || h.is_comment_char(stream.peek_char())) {
                h.skip_comments_and_spaces_if();
                if (!stream.alive() || stream.peek_char() == ')') throw "\\. syntax error";
                result.push_back(h.read());
                h.skip_comments_and_spaces_if();
                if(!stream.alive() || stream.peek_char() != ')') throw "\\. syntax error";
                pair = true;
                continue;
            }
            else {
                stream.unread_last();
            }
        }

        result.push_back(h.read());
    }
    throw throw_token_stream_empty{};
}

Cell empty_macrochar(read_helper& h) {
    throw "syntax error";
}



SExprStream::SExprStream(Syntaxer& s) :
    t_helper(s)
{
}

Cell SExprStream::read()
{
    return t_helper.read();
}

bool SExprStream::alive() const
{
    return t_helper.get_stream().alive();
}

bool SExprStream::ready() const
{
    return t_helper.get_stream().alive();
}

read_helper::read_helper(Syntaxer& s):
    t_s(s)
{

}

Cell read_helper::read()
{
    string result;
    bool ruc = t_s.t_read_upcase_mode();
    auto& stream = t_s.t_stream();
    while (stream.alive()) {
        auto ch = stream.get_char();
        if (ch == '\\') {
            if (!stream.alive()) throw throw_token_stream_empty{};
            result += stream.get_char();
            continue;
        }
        if (is_macro_char(ch) || (!empty(result) && ch == '\n' && (stream.get_mode() == CoreData::stream_read_mode::new_string))) {
            if (empty(result)) {
                return t_read_macro_char(ch);
            }
            stream.unread_last();
            break;
        }
        if (is_space_char(ch)) {
            if (empty(result)) {
                stream.unread_last();
                t_skip_spaces_if();
                continue;
            }
            stream.unread_last();
            break;
        }
        if (is_comment_char(ch)) {
            stream.unread_last();
            t_skip_comments_if();
            continue;
        }
        if (is_number_char(ch)) {
            if (empty(result) || result.size() == 1 && (result[0] == '-' || result[0] == '+')) {
                result += ch;
                return t_read_number(move(result));
            }
            result += ch;
            continue;
        }
        result += (ruc) ? toupper(static_cast<int>(ch)) : ch;
    }
    return t_s.t_farm().make_symbol_cell(move(result));
}

bool read_helper::is_macro_char(char ch) const
{
    return t_s.t_mc.is_macro_char(ch);
}

bool read_helper::is_space_char(char ch) const
{
    if (ch == '\n') {
        return true;
        //return t_s.t_stream().get_mode() == CoreData::stream_read_mode::s_expression;
    }
    return skip_symbols.find(ch) != end(skip_symbols);
}

bool read_helper::is_comment_char(char ch) const
{
    return ch == ';';
}

bool read_helper::is_number_char(char ch) const
{
    if (isdigit(static_cast<unsigned char>(ch))) {
        if (t_s.t_number_base_mode() >= 10) return true;
        return (ch - '0') < t_s.t_number_base_mode();
    }
    return false;
}

const CoreInputStreamInt& read_helper::get_stream() const
{
    return t_s.t_stream();
}

CoreInputStreamInt& read_helper::get_stream()
{
    return t_s.t_stream();
}

SExprsFarm& read_helper::get_farm()
{
    return t_s.t_farm();
}

bool read_helper::is_skip_comment() const
{
    return t_s.t_skip_comments_mode();
}

void read_helper::skip_comments_and_spaces_if()
{
    while (t_s.t_stream().alive()) {
        if (!t_skip_comments_if() && !t_skip_spaces_if()) break;
    }
}

Cell read_helper::t_read_macro_char(char ch)
{
    if (!is_macro_char(ch)) throw "t_read_macro_char: ch is't macrochar";
    const auto& ref = t_s.t_mc.get_macro_char(ch);
    if (holds_alternative<Cell>(ref)) {
        auto copy = Cell(get<Cell>(ref));
        auto buf = t_s.t_farm().make_list_cell(copy);
        return t_s.t_env.t_execute(buf);
    }
    else {
        return get<std::function<Cell(read_helper&)>>(ref)(*this);
    }
}

Cell read_helper::t_read_number(string&& prev)
{
    bool dot = false;
    string result = move(prev);
    auto& stream = t_s.t_stream();
    while (stream.alive()) {
        auto ch = stream.get_char();
        if ((ch == '\n' && (stream.get_mode() == CoreData::stream_read_mode::new_string))) {
            break;
        }
        if (is_space_char(ch)) {
            if (empty(result)) {
                stream.unread_last();
                t_skip_spaces_if();
                continue;
            }
            stream.unread_last();
            break;
        }
        if (is_comment_char(ch)) {
            stream.unread_last();
            t_skip_comments_if();
            continue;
        }
        if (!(is_number_char(ch) || (ch =='.' && !dot))) {
            stream.unread_last();
            break;
        }
        else if (ch == '.') {
            if (stream.alive() && is_number_char(stream.peek_char())) {
                result += '.';
                continue;
            }
            else {
                stream.unread_last();
                break;
            }
        }
        result += ch;
    }

    if (is_real_number(result.c_str())) {
        stringstream s(result + " ");
        double n;
        s >> n;
        if (s.good())
        {
            return t_s.t_farm().make_number_cell(n);
        }
        else
        {
            result.erase(result.find('.'), result.npos);
            return t_s.t_farm().make_number_cell(BigInt(move(result)));
        }
    }
    if (is_integer_number(result.c_str())) {
        if (result[0] == '+') result.erase(begin(result));
        return t_s.t_farm().make_number_cell(BigInt(move(result)));
    }
    else if (is_half_integer_num(result.c_str())) {
        if (result[0] == '+') result.erase(begin(result));
        result.erase(result.find('.'), result.npos);
        return t_s.t_farm().make_number_cell(BigInt(move(result)));
    }
    else {
        throw "invalid input";
    }
}

bool read_helper::t_skip_comments_if()
{
    auto& stream = t_s.t_stream();
    if (stream.peek_char() == ';') {
        auto ch = stream.get_char();
        while (stream.alive() && ch != '\n') {
            ch = stream.get_char();
        }
        return true;
    }
    else {
        return false;
    }
}

bool read_helper::t_skip_spaces_if()
{
    auto& stream = t_s.t_stream();
    if (is_space_char(stream.peek_char())) {
        auto ch = stream.get_char();
        while (stream.alive() && is_space_char(ch)) {
            ch = stream.get_char();
        }
        if(!is_space_char(ch)) stream.unread_last();
        return true;
    }
    else {
        return false;
    }
}


MacroChars::MacroChars()
{
    t_macrochars.emplace('(', &macrochar_left_round_braket);
    t_macrochars.emplace('"', &macrochar_double_quote);
    t_macrochars.emplace('|', &macrochar_vertical_line);
    t_macrochars.emplace(')', &empty_macrochar);
    t_macrochars.emplace('\'',&empty_macrochar);
    t_macrochars.emplace(',', &empty_macrochar);
}

void MacroChars::set_macro_char(char ch, MacroChars::cont_type&& defenition) {
    t_macrochars[ch] = move(defenition);
}

const MacroChars::cont_type& MacroChars::get_macro_char(char ch) const{
    auto it = t_macrochars.find(ch);
    if (it == end(t_macrochars)) throw "get_macro_char: null";
    return it->second;
}

bool MacroChars::is_macro_char(char ch) const {
    return t_macrochars.find(ch) != end(t_macrochars);
}


Syntaxer::Syntaxer(CoreEnvironment& env) :
    t_env(env),
    t_rap_buf(env.t_farm.make_symbol_cell(CoreData::read_up_case_str))
{
}

Cell Syntaxer::read_sexpr()
{
    read_helper h(*this);
    return h.read();
}

SExprStream Syntaxer::read_sexprs_stream()
{
    return SExprStream(*this);
}

Cell Syntaxer::listen() const {
    return bool_cast(t_stream().alive(), t_env.t_farm);
}

Cell Syntaxer::read_char(const Cell& flag) {
    if (!t_stream().alive()) throw "read_char: empty stream";
    if (!is_null(flag)) {
        return t_env.t_farm.make_symbol_cell(string() + t_stream().peek_char());
    }
    return t_env.t_farm.make_symbol_cell(string() + t_stream().get_char());
    //read-atom val
}

Cell Syntaxer::peek_char(const Cell& s) {
    if (!t_stream().alive()) throw "peek_char: empty stream";
    if (is_symbol(s) && !is_null(s)) {
        read_helper h(*this);
        if (to_symbol(s) == CoreData::T_str) {
            h.skip_comments_and_spaces_if();
            return t_env.t_farm.make_symbol_cell(string() + t_stream().peek_char());
        }
        else {
            auto raw = t_env.t_output_control.to_string_raw(s);
            if (empty(raw)) throw "empty symbol";
            auto symbol = raw[0];
            auto ch = t_stream().get_char();
            while (ch != symbol) {
                ch = t_stream().get_char();
            }
            t_stream().unread_last();
            return t_farm().make_symbol_cell(string() + ch);
        }
    }
    else {
        return t_env.t_farm.make_symbol_cell(string() + t_stream().peek_char());
    }
    //read-atom val
}

void Syntaxer::unread_char() {
    t_stream().unread_last();
}

MacroChars& Syntaxer::get_macrochars()
{
    return t_mc;
}

SExprsFarm& Syntaxer::t_farm()
{
    return t_env.t_farm;
}

CoreInputStreamInt& Syntaxer::t_stream()
{
    return *t_env.t_cis;
}

CoreInputStreamInt& Syntaxer::t_stream() const
{
    return t_env.t_streams->get().t_input_stream();
}

bool Syntaxer::t_read_upcase_mode()
{
    auto c = t_env.t_envs.get(to_symbol(t_rap_buf));
    return c.empty() || !is_null(c);
}

bool Syntaxer::t_skip_comments_mode()
{
    //
    return true;
}

int Syntaxer::t_number_base_mode()
{
    //
    return 10;
}
