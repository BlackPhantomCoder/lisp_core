#include "CoreData.h"
#include "CoreEnv.h"
#include "BiFuncs.h"
#include "SimpleBifuncs.h"

#include <algorithm>

using namespace std;
using namespace CoreData;

template<class T>
CoreData::HolderPtr make_special_bifunc(CarCdrIterator b, CarCdrIterator e, bool forse_noeval) {
    return make_fnc<T>(b, e, forse_noeval);
}

template<class T>
CoreData::HolderPtr make_special_nbifunc(CarCdrIterator b, CarCdrIterator e) {
    return make_fnc<T>(b, e);
}


bifuncs_array CoreData::bifunc_setup()
{
    return {
        make_pair("CAR", &SBIF::bifunc_car),
        make_pair("CDR", &SBIF::bifunc_cdr),
        make_pair("CONS", &SBIF::bifunc_cons),
        make_pair("LIST", &SBIF::bifunc_list),
        make_pair("NULL", &SBIF::bifunc_null),
        make_pair("NUMBERP", &SBIF::bifunc_numberp),
        make_pair("SYMBOLP", &SBIF::bifunc_symbolp),
        make_pair("LISTP", &SBIF::bifunc_listp),
        make_pair("ATOM", &SBIF::bifunc_atomp),
        make_pair("+", &SBIF::bifunc_add),
        make_pair("-", &SBIF::bifunc_sub),
        make_pair("*", &SBIF::bifunc_mul),
        make_pair("/", &SBIF::bifunc_div),
        make_pair(">", &SBIF::bifunc_greater),
        make_pair("<", &SBIF::bifunc_less),
        make_pair("<=", &SBIF::bifunc_less_equal),
        make_pair(">=", &SBIF::bifunc_greater_equal),
        make_pair("=", &SBIF::bifunc_num_eq),
        make_pair("GETD", &SBIF::bifunc_getd),
        make_pair("SET", &SBIF::bifunc_set),
        make_pair("PRINT", &SBIF::bifunc_print),
        make_pair("PRIN1", &SBIF::bifunc_prin1),
        make_pair("EQUAL", &SBIF::bifunc_equal),
        make_pair("LAST", &SBIF::bifunc_last),
        make_pair("LENGTH", &SBIF::bifunc_length),
        make_pair("PACK", &SBIF::bifunc_pack),
        make_pair("UNPACK", &SBIF::bifunc_unpack),
        make_pair("EQ", &SBIF::bifunc_eq),
        // make_pair("MOD", &SBIF::bifunc_mod),
        make_pair("INTEGERP", &SBIF::bifunc_integerp),
        make_pair("OBLIST", &SBIF::bifunc_oblist),
        make_pair("RPLACA", &SBIF::bifunc_rplaca),
        make_pair("RPLACD", &SBIF::bifunc_rplacd),
        make_pair("COPY-TREE", &SBIF::bifunc_copy_tree),
        make_pair("READ-CHAR", &SBIF::bifunc_read_char),
        make_pair("UNREAD-CHAR", &SBIF::bifunc_unread_char),
        //make_pair("PEEK-CHAR", &SBIF::bifunc_peek_char),
        make_pair("LISTEN", &SBIF::bifunc_listen),
        make_pair("BREAK", &SBIF::bifunc_break),
        make_pair("GET-MACRO-CHAR", &SBIF::bifunc_get_macro_char),
        make_pair("SET-MACRO-CHAR", &SBIF::bifunc_set_macro_char)
    };
}

nbifuncs_array CoreData::nbifunc_setup()
{
    return CoreData::nbifuncs_array{
        make_pair("DEFUN", &SBIF::nbifunc_defun),
        make_pair("QUOTE", &SBIF::nbifunc_quote),
        make_pair("DEFMACRO", &SBIF::nbifunc_defmacro),
    };
}

special_bifuncs_array CoreData::special_bifunc_setup()
{
    return CoreData::special_bifuncs_array{
        make_pair("EVAL", &make_special_bifunc<Eval>),
        make_pair("READ", &make_special_bifunc<Read>),
        make_pair("APPLY", &make_special_bifunc<Apply>),
        make_pair("APPEND", &make_special_bifunc<Append>),
        make_pair("MACROEXPAND-1", &make_special_bifunc<MacroExpand1>),
        make_pair("MACROEXPAND", &make_special_bifunc<MacroExpand>),
        make_pair("MAPCAR", &make_special_bifunc<MapCar>),
        make_pair("PEEK-CHAR", &make_special_bifunc<PeekChar>),
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

namespace CoreData{
    const bifuncs_array bifuncs_arr = bifunc_setup();
    const nbifuncs_array nbifuncs_arr = nbifunc_setup();
    const special_bifuncs_array special_bifuncs_arr = special_bifunc_setup();
    const special_nbifuncs_array special_nbifuncs_arr = special_nbifunc_setup();
}

funcs_pool& CoreData::clear_pool_funcs()
{
    static funcs_pool p;
    return p;
}

void CoreData::funcs_pools_clear()
{
    clear_pool_funcs().clear();
}