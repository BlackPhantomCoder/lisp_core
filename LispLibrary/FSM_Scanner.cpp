#include "FSM_Scanner.h"
#include "CoreEnv.h"

FSM_scanner::FSM_scanner(CoreEnvironment& env):
    t_env(env)
{

}

Input& FSM_scanner::t_input()
{
    return t_env.input();
}

MacroTable& FSM_scanner::t_macrotable()
{
    return t_env.macrotable();
}
