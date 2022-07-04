#pragma once
#include <string>
#include <functional>

class CoreEnvironment;

class FSM_Symbols {
public:
    enum class type {
        symbol, none
    };
private:
    enum class states {
        start, escape, accept
    };
public:
    FSM_Symbols(CoreEnvironment& env);

    //EOF symbol (if needed)
    bool at_end()const;

    void restart();
    // reject/no
    bool next(char ch);
    // is in Terminal
    bool is_T() const;
    // возвращает чем является объект
    // none == не терминальная вершина
    type get_T_type() const;
private:
    states t_state = states::start;
    CoreEnvironment& t_env;
};
