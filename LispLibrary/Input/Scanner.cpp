#include "Scanner.h"

#include "BigInt.h"
#include "SExprsFarm.h"
#include "parser.h"

#include "CoreEnv.h"
#include <algorithm>
#include <locale>

using namespace std;

// число по ch по заданной сс base
int get_by_base(char ch, int base) {
    if (ch >= 'A' && ch <= 'Z') {
        if (base > (ch - 'A' + 10)) {
            return ch - 'A' + 10;
        }
    }
    else if (ch >= '0' && ch <= '9') {
        if (base > ch - '0') {
            return ch - '0';
        }
    }
    throw logic_error("get_by_base");
}

//целое число из прочитанной строки
BigInt z_numb_from_str(std::string_view str, int base)
{
    auto result = BigInt(0l);
    auto i = 0;
    bool minus = str[0] == '-';
    for (auto it = rbegin(str); it != rend(str) + ((minus) ? -1 : 0); ++it, ++i) {
        result += get_by_base(*it, base) * pow(base, i);
    }
    if (minus) result *= (-1l);
    return result;
}

//дробное число из прочитанной строки
double r_numb_from_str(std::string_view str, int base)
{
    auto result = 0.;
    auto i = 0;
    auto dot_it = find(begin(str), end(str), '.');
    bool minus = str[0] == '-';
    for (auto it = reverse_iterator{ dot_it }; it != rend(str) + ((minus) ? -1 : 0); ++it, ++i) {
        result += get_by_base(*it, base) * pow(base, i);
    }
    i = -1;
    for (auto it = next(dot_it); it != end(str); ++it, --i) {
        result += get_by_base(*it, base) * pow(base, i);
    }
    if (minus) result *= (-1);
    return result;
}

//строка из прочитанной строки
string str_from_str(string_view str) {
    auto buf = string();
    buf.reserve(str.size() - 2);
    bool escape = false;
    for (auto it = begin(str) + 1; it != end(str) - 1; ++it) {
        if (escape) {
            buf += *it;
            escape = false;
        }
        else {
            if (*it == '\\') {
                escape = true;
            }
            else {
                buf += *it;
            }
        }
    }
    buf.shrink_to_fit();
    return buf;
}


//символ из прочитанной строки
string symbol_from_str(string_view data, bool upper) {
    auto buf = string();
    buf.reserve(data.size());
    auto it = begin(data);
    auto escape = false;
    for (const auto& ch : data) {
        if (escape) {
            buf += ch;
            escape = false;
        }
        else {
            if (ch == '\\') {
                escape = true;
            }
            else {
                if (upper)
                    buf += toupper(ch, locale());
                else
                    buf += ch;
            }
        }
    }
    buf.shrink_to_fit();
    return buf;
}



Scanner::Scanner(CoreEnvironment& env) :
    t_env(env),
    t_main_fsm(),
    t_fsm_scanner(t_env),
    t_numb_fsm([this]() {return  t_env.input().read_base_val(); }),
    t_nonmacros_fsm(),
    t_symbols_fsm(t_env)
{

}

std::pair<int, std::optional<Cell>> Scanner::lex()
{
    while (true) {
        auto i = t_env.input().read_char();
        if (i == -1) {
            return pair{ tokens::YYEOF, nullopt };
        }

        //проверка специальных символов и макросимволов
        auto ch = char(i);
        switch (ch)
        {
        case '.':
            //'.' в контексте '('
            if (t_env.macrotable().is_macro_char('(')) {
                //если переоределён
                if (auto opt = t_env.macrotable().is_macro_char('.'); opt)
                    return t_make_macro(opt->get());
            }
            break;
        case ')':
            //')' в контексте '('
            if (t_env.macrotable().is_macro_char('(')) {
                //если переоределён
                if (auto opt = t_env.macrotable().is_macro_char(')'); opt)
                    return t_make_macro(opt->get());
            }
            return pair{ i, std::optional<Cell>{} };
        case ']':
            //']' в контексте '('
            if (t_env.macrotable().is_macro_char('(')) {
                //если переоределён
                if (auto opt = t_env.macrotable().is_macro_char(']'); opt)
                    return t_make_macro(opt->get());
            }
            return pair{ i, std::optional<Cell>{} };
        case '\'':
        case '(':
        case ',':
            //если переоределён
            if (auto opt = t_env.macrotable().is_macro_char(ch); opt)
                return t_make_macro(opt->get());
            return pair{ i, std::optional<Cell>{} };
        default:
            //если переоределён
            if (auto opt = t_env.macrotable().is_macro_char(ch); opt)
                return t_make_macro(opt->get());
            break;
        }
        auto [type, data] = t_fsm_scanner.scan<true>(t_main_fsm, ""s + ch);
        if (type == FSM_scanner::scan_result_var::token) {
            switch (t_main_fsm.get_T_id()) {
                case FSM::T_id::witespace:
                    continue;
                case FSM::T_id::symbol:
                    tie(type, data) = t_fsm_scanner.scan<true>(t_symbols_fsm, data);
                    return t_make_symbol(data);
                case FSM::T_id::numb_or_symbol:
                    {
                        auto number = false;
                        try
                        {
                            tie(type, data) = t_fsm_scanner.scan<false>(t_numb_fsm, data);
                            number = true;
                        }
                        catch (errors::lexical_error& e) {}
                        catch (errors::eos& e) {}
                        if (number) {
                            return t_make_number(t_numb_fsm.get_T_type(), data);
                        }
                        else {
                            try
                            {
                                tie(type, data) = t_fsm_scanner.scan<false>(t_symbols_fsm, data);
                            }
                            catch (errors::lexical_error& e) {}
                            catch (errors::eos& e) {}
                            return t_make_symbol(data);
                        }
                    }
                case FSM::T_id::numb:
                    tie(type, data) = t_fsm_scanner.scan<false>(t_numb_fsm, data);
                    if (type != FSM_scanner::scan_result_var::token) break;
                    return t_make_number(t_numb_fsm.get_T_type(), data);
                case FSM::T_id::non_macros:
                    {
                        tie(type, data) = t_fsm_scanner.scan<false>(t_nonmacros_fsm, data);
                        if (type != FSM_scanner::scan_result_var::token) break;
                        switch (t_nonmacros_fsm.get_T_id()) {
                            case FSM_nonmacros::T_id::comment:
                                //
                                continue;
                            case FSM_nonmacros::T_id::str1:
                            case FSM_nonmacros::T_id::str2:
                                return pair{ tokens::str, t_env.farm().make_symbol_cell(str_from_str(data)) };
                            default:
                                continue;
                        }
                    }
                default:
                    continue;
            }
        }
        if (type == FSM_scanner::scan_result_var::macros) {
                return t_make_macro(t_env.macrotable().is_macro_char(data[0])->get());
        }
        else if (type == FSM_scanner::scan_result_var::eos) {
            return pair{ tokens::YYEOF, nullopt };
        }
    }
}

std::pair<int, std::optional<Cell>> Scanner::t_make_macro(const macro_char& ch)
{
    auto copy = Cell(ch.func);
    return pair{ tokens::Macro, t_env.farm().make_list_cell(copy) };
}

std::pair<int, std::optional<Cell>> Scanner::t_make_number(FSM_numb::type type, const std::string& data)
{
    switch (type) {
    case FSM_numb::type::r_numb:
        return
            pair{
                tokens::r_numb,
                t_env.farm().make_number_cell(r_numb_from_str(data, t_env.input().read_base_val()))
        };
    case FSM_numb::type::z_numb:
        return
            pair{
                tokens::z_numb,
                t_env.farm().make_number_cell(z_numb_from_str(data, t_env.input().read_base_val()))
        };
    default:
        throw logic_error("Scanner::t_make_number");
    }
}

std::pair<int, std::optional<Cell>> Scanner::t_make_symbol(const std::string& data)
{
    // специальный символ
    if (data == ".") return pair{ int('.'), std::optional<Cell>{} };
    return
        pair{
            tokens::symbol,
            t_env.farm().make_symbol_cell(symbol_from_str(data,t_env.input().read_upcase_val()))
    };
}
