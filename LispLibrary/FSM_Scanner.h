#pragma once

#include <optional>
#include <string>
#include <functional>
#include <variant>
#include "Errors.h"
#include "Input/MacroTable.h"
#include "Input.h"

class CoreEnvironment;

// для разбора с помощью FSM
class FSM_scanner {
public:
    enum class scan_result_var { token, macros, eos };
    using result_type = std::pair<scan_result_var, std::string>;
public:
    FSM_scanner(CoreEnvironment& env);

    // разбор
    // macros - если нужно проверять ограничивающие макросимволы
    template<bool macros, class FSM>
    result_type scan(FSM& fsm, const std::string& init);
private:
    Input& t_input();
    MacroTable& t_macrotable();
private:
    CoreEnvironment& t_env;
};













template<bool macros, class FSM>
FSM_scanner::result_type FSM_scanner::scan(FSM& fsm, const std::string& init)
{
    if (empty(init)) throw std::logic_error("FSM_scanner::scan");
    // for macros
    auto escape = false;

    auto data = std::string(init);
    fsm.restart();

    for (auto& ch : data) {
        if (!fsm.next(ch)) {
            throw errors::lexical_error{};
        }
    }
    // for macros
    if constexpr (macros) {
        escape = data.back() == '\\';
    }

    if (t_input().alive()) {
        auto unread = true;
        while (true) {
            if (auto i = t_input().read_char(); i != -1) {
                auto ch = char(i);
                // for macros
                if constexpr (macros) {
                    if (!escape) {
                        auto opt = t_macrotable().is_macro_char(ch);
                        if (opt && (opt->get().is_terminating() || opt->get().is_comment())) {
                            break;
                        }
                    }
                }

                if (fsm.next(ch)) {
                    data += ch;
                    // for macros
                    if constexpr (macros) {
                        if (escape) escape = false;
                        else if (ch == '\\') escape = true;
                    }
                    continue;
                }
            }
            else {
                //stream set eos
                unread = false;
                fsm.at_end();
            }
            break;
        }
        if(unread)
            t_input().unread_char();
    }
    if (!fsm.is_T()) {
        throw errors::lexical_error{};
    }
    return { scan_result_var::token, std::move(data) };
}