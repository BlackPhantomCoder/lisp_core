#include "FSM_Symbols.h"
#include "CoreEnv.h"
#include <unordered_set>
using namespace std;

namespace {
    const unordered_set<char> abort_symbols = {' ', '\t', '\n', '\r', ';', '\"', '|',  '(', ')', '\'', ']'};
}


FSM_Symbols::FSM_Symbols(CoreEnvironment& env):
    t_env(env)
{
}

bool FSM_Symbols::at_end() const
{
    return true;
}

void FSM_Symbols::restart()
{
    t_state = states::start;
}

bool FSM_Symbols::next(char ch)
{
    switch (t_state)
    {
    case FSM_Symbols::states::escape:
        t_state = states::accept;
        return true;
    case FSM_Symbols::states::start:
    case FSM_Symbols::states::accept:
        if (ch == '\\') {
            t_state = states::escape;
            return true;
        }
        else {
            if (abort_symbols.find(ch) == end(abort_symbols)) {
                t_state = states::accept;
                return true;
            }
            else if (auto opt = t_env.macrotable().is_macro_char(ch); opt && opt->get().is_non_terminating()) {
                t_state = states::accept;
                return true;
            }
        }
        break;
    default:
        break;
    }

    return false;
}

bool FSM_Symbols::is_T() const
{
    return t_state == states::accept;
}

FSM_Symbols::type FSM_Symbols::get_T_type() const
{
    if (t_state == states::accept) return type::symbol;
    return type::none;
}
