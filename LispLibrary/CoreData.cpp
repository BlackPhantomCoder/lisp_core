#include "CoreData.h"
#include "CoreEnv.h"
#include "BiFuncs.h"

#include <algorithm>

using namespace std;
using namespace CoreData;

template<class T>
CoreData::HolderPtr make_special_bifunc(CoreEnvironment& env, CarCdrIterator b, CarCdrIterator e, bool forse_noeval) {
    return make_fnc<T>(env, b, e, forse_noeval);
}

template<class T>
CoreData::HolderPtr make_special_nbifunc(CoreEnvironment& env, CarCdrIterator b, CarCdrIterator e) {
    return make_fnc<T>(env, b, e);
}


bifuncs_array CoreData::bifunc_setup()
{
    return {
        make_pair("CAR", &CoreEnvironment::bifunc_car),
        make_pair("CDR", &CoreEnvironment::bifunc_cdr),
        make_pair("CONS", &CoreEnvironment::bifunc_cons),
        make_pair("LIST", &CoreEnvironment::bifunc_list),
        make_pair("NULL", &CoreEnvironment::bifunc_null),
        make_pair("NUMBERP", &CoreEnvironment::bifunc_numberp),
        make_pair("SYMBOLP", &CoreEnvironment::bifunc_symbolp),
        make_pair("LISTP", &CoreEnvironment::bifunc_listp),
        make_pair("ATOM", &CoreEnvironment::bifunc_atomp),
        make_pair("+", &CoreEnvironment::bifunc_add),
        make_pair("-", &CoreEnvironment::bifunc_sub),
        make_pair("*", &CoreEnvironment::bifunc_mul),
        make_pair("/", &CoreEnvironment::bifunc_div),
        make_pair(">", &CoreEnvironment::bifunc_greater),
        make_pair("<", &CoreEnvironment::bifunc_less),
        make_pair("<=", &CoreEnvironment::bifunc_less_equal),
        make_pair(">=", &CoreEnvironment::bifunc_greater_equal),
        make_pair("=", &CoreEnvironment::bifunc_num_eq),
        make_pair("GETD", &CoreEnvironment::bifunc_getd),
        make_pair("SET", &CoreEnvironment::bifunc_set),
        make_pair("READ", &CoreEnvironment::bifunc_read),
        make_pair("PRINT", &CoreEnvironment::bifunc_print),
        make_pair("PRIN1", &CoreEnvironment::bifunc_prin1),
        make_pair("EQUAL", &CoreEnvironment::bifunc_equal),
        make_pair("LAST", &CoreEnvironment::bifunc_last),
        make_pair("LENGTH", &CoreEnvironment::bifunc_length),
        make_pair("PACK", &CoreEnvironment::bifunc_pack),
        make_pair("UNPACK", &CoreEnvironment::bifunc_unpack),
        make_pair("EQ", &CoreEnvironment::bifunc_eq),
        // make_pair("MOD", &CoreEnvironment::bifunc_mod),
        make_pair("INTEGERP", &CoreEnvironment::bifunc_integerp),
        make_pair("OBLIST", &CoreEnvironment::bifunc_oblist),
        make_pair("RPLACA", &CoreEnvironment::bifunc_rplaca),
        make_pair("RPLACD", &CoreEnvironment::bifunc_rplacd),
        make_pair("COPY-TREE", &CoreEnvironment::bifunc_copy_tree),
        make_pair("READ-CHAR", &CoreEnvironment::bifunc_read_char),
        make_pair("UNREAD-CHAR", &CoreEnvironment::bifunc_unread_char),
        make_pair("PEEK-CHAR", &CoreEnvironment::bifunc_peek_char),
        make_pair("LISTEN", &CoreEnvironment::bifunc_listen),
        make_pair("BREAK", &CoreEnvironment::bifunc_break),
        make_pair("GET-MACRO-CHAR", &CoreEnvironment::bifunc_get_macro_char),
        make_pair("SET-MACRO-CHAR", &CoreEnvironment::bifunc_set_macro_char)
    };
}

nbifuncs_array CoreData::nbifunc_setup()
{
    return CoreData::nbifuncs_array{
        make_pair("DEFUN", &CoreEnvironment::nbifunc_defun),
        make_pair("QUOTE", &CoreEnvironment::nbifunc_quote),
        make_pair("DEFMACRO", &CoreEnvironment::nbifunc_defmacro),
    };
}

special_bifuncs_array CoreData::special_bifunc_setup()
{
    return CoreData::special_bifuncs_array{
        make_pair("EVAL", &make_special_bifunc<Eval>),
        make_pair("APPLY", &make_special_bifunc<Apply>),
        make_pair("APPEND", &make_special_bifunc<Append>),
        make_pair("MACROEXPAND-1", &make_special_bifunc<MacroExpand1>),
        make_pair("MACROEXPAND", &make_special_bifunc<MacroExpand>),
        make_pair("MAPCAR", &make_special_bifunc<MapCar>),
    };
}

special_nbifuncs_array CoreData::special_nbifunc_setup()
{
    return CoreData::special_nbifuncs_array{
       make_pair("COND", &make_special_nbifunc<Cond>),
       make_pair("PROGN", &make_special_nbifunc<ProgN>),
       make_pair("PROG1", &make_special_nbifunc<Prog1>),
       make_pair("LOOP", &make_special_nbifunc<Loop>),
       make_pair("SETQ", &make_special_nbifunc<SetQ>),
    };
}

std::vector<std::function<void(void)>>& CoreData::clear_pool_funcs()
{
    static std::vector<std::function<void(void)>> p;
    return p;
}

void CoreData::funcs_pools_clear()
{
    for (auto& fnc : CoreData::clear_pool_funcs()) {
        fnc();
    }
}