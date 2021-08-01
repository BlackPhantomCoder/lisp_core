#include "CoreData.h"
#include "CoreEnv.h"
#include <algorithm>

using namespace std;
using namespace CoreData;

bifuncs_array CoreData::bifunc_setup()
{
    auto result = bifuncs_array{
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
        make_pair("PROG1", &CoreEnvironment::bifunc_prog1),
        make_pair("PRINT", &CoreEnvironment::bifunc_print),
        make_pair("EQUAL", &CoreEnvironment::bifunc_equal),
        make_pair("APPLY", &CoreEnvironment::bifunc_apply),
        make_pair("LAST", &CoreEnvironment::bifunc_last),
        make_pair("LENGTH", &CoreEnvironment::bifunc_length),
        //make_pair("PACK", &CoreEnvironment::bifunc_pack),
        //make_pair("UNPACK", &CoreEnvironment::bifunc_unpack),
        make_pair("EQ", &CoreEnvironment::bifunc_eq),
        // make_pair("MOD", &CoreEnvironment::bifunc_mod),
        make_pair("EVAL", &CoreEnvironment::bifunc_eval),
        make_pair("APPEND", &CoreEnvironment::bifunc_append),
        make_pair("INTEGERP", &CoreEnvironment::bifunc_integerp),
        make_pair("OBLIST", &CoreEnvironment::bifunc_oblist),
    };

    std::sort(
        begin(result),
        end(result),
        bifunc_pair_less{}
    );

    return result;
}

nbifuncs_array CoreData::nbifunc_setup()
{
    auto result = CoreData::nbifuncs_array{
        make_pair("DEFUN", &CoreEnvironment::nbifunc_defun),
        make_pair("QUOTE", &CoreEnvironment::nbifunc_quote),
        make_pair("COND", &CoreEnvironment::nbifunc_cond),
        make_pair("PROGN", &CoreEnvironment::nbifunc_progn),
        make_pair("SETQ", &CoreEnvironment::nbifunc_setq),
        make_pair("LOOP", &CoreEnvironment::nbifunc_loop),
    };

    std::sort(
        begin(result),
        end(result),
        bifunc_pair_less{}
    );
    return result;
}

bool CoreData::bifunc_pair_less::operator()(const pair<const char*, CoreData::bifunc>& lh, const pair<const char*, CoreData::bifunc>& rh) const
{
    return strcmp(lh.first,rh.first) < 0;
}

bool CoreData::bifunc_pair_less_w_string::operator()(const pair<const char*, CoreData::bifunc>& lh, const Symbol& rh) const
{
    return strcmp(lh.first, rh.to_string()) < 0;
}

