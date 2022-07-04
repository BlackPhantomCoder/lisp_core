#pragma once
#include <unordered_set>

namespace tokens {
    extern const int YYEOF;
    extern const int str;
    extern const int symbol;
    extern const int z_numb;
    extern const int r_numb;
    extern const int Macro;
    extern const int FinalMacro;

    extern const int YYPUSH_MORE;
}


struct yypstate;
typedef struct yypstate yypstate;

extern "C" yypstate * yypstate_new(void);
extern "C" void yypstate_delete(yypstate * ps);
extern "C" int yypush_parse(yypstate * ps, int pushed_char, unsigned long const* pushed_val, void* lexer);