#pragma once
#include "Input/FSM_Num/FSM_Num.h"
#include "Input/FSM/FSM.h"
#include "Input/FSM/FSM_nonmacros.h"
#include "Cell.h"

#include "FSM_Scanner.h"
#include "FSM_Symbols.h"
#include <optional>

class CoreEnvironment;

class Scanner  {
public:
    Scanner(CoreEnvironment& env);

    // сканируем токен
    std::pair<int, std::optional<Cell>> lex();
private:
    std::pair<int, std::optional<Cell>> t_make_macro(const macro_char& ch);
    std::pair<int, std::optional<Cell>> t_make_number(FSM_numb::type type, const std::string& data);
    std::pair<int, std::optional<Cell>> t_make_symbol(const std::string& data);
private:
    CoreEnvironment& t_env;
    FSM_scanner t_fsm_scanner;
    FSM t_main_fsm;
    FSM_numb t_numb_fsm;
    FSM_nonmacros t_nonmacros_fsm;
    FSM_Symbols t_symbols_fsm;
};