#include "FuncID.h"
using namespace std;

const std::array<func_data, (unsigned char)func_id::end_id> func_table = []() {
    auto result = std::array<func_data, (unsigned char)func_id::end_id>();

	result[(unsigned char)func_id::prog1] = make_range_nbifunc_data<Prog1>();
	result[(unsigned char)func_id::progn] = make_range_nbifunc_data<ProgN>();
	result[(unsigned char)func_id::eval] = make_func_data<Eval,false, false>();
	result[(unsigned char)func_id::cond] = make_range_nbifunc_data<Cond>();
	result[(unsigned char)func_id::append] = make_func_data<Append,false, true>();
	result[(unsigned char)func_id::loop] = make_range_nbifunc_data<Loop>();
	result[(unsigned char)func_id::apply] = make_func_data<Apply,false, true>();
	result[(unsigned char)func_id::setq] = make_range_nbifunc_data<SetQ>();
	result[(unsigned char)func_id::macroexpand1] = make_func_data<MacroExpand1,false, true>();
	result[(unsigned char)func_id::macroexpand] = make_func_data<MacroExpand,false, true>();
	result[(unsigned char)func_id::mapcar] = make_func_data<MapCar,false, true>();
	result[(unsigned char)func_id::read] = make_func_data<Read,true, false>();
	result[(unsigned char)func_id::peekchar] = make_func_data<PeekChar,false, true>();


	result[(unsigned char)func_id::evallambda] = make_func_data<EvalLambda,false, false>();
	result[(unsigned char)func_id::evalquote] = make_func_data<EvalQuote,false, false>();
	result[(unsigned char)func_id::evalquoterange] = make_range_nbifunc_data<EvalQuoteRange>();
	result[(unsigned char)func_id::implicitcond] = make_func_data<ImplicitCond,false, false>();
	result[(unsigned char)func_id::evalfunc] = make_func_data<EvalFunc,false, false>();
	result[(unsigned char)func_id::expandmacro] = make_range_nbifunc_data<ExpandMacro>();
	result[(unsigned char)func_id::evalmacro] = make_func_data<EvalMacro,false, false>();


	return result;
}();

void free_pool_by_funcid(Func* func)
{
    if (func) {
        switch (func->id())
        {
        case func_id::prog1:
            free_pool_func<Prog1>(func);
            break;
        case func_id::progn:
            free_pool_func<ProgN>(func);
            break;
        case func_id::eval:
            free_pool_func<Eval>(func);
            break;
        case func_id::cond:
            free_pool_func<Cond>(func);
            break;
        case func_id::append:
            free_pool_func<Append>(func);
            break;
        case func_id::loop:
            free_pool_func<Loop>(func);
            break;
        case func_id::apply:
            free_pool_func<Apply>(func);
            break;
        case func_id::setq:
            free_pool_func<SetQ>(func);
            break;
        case func_id::macroexpand1:
            free_pool_func<MacroExpand1>(func);
            break;
        case func_id::macroexpand:
            free_pool_func<MacroExpand>(func);
            break;
        case func_id::mapcar:
            free_pool_func<MapCar>(func);
            break;
        case func_id::read:
            free_pool_func<Read>(func);
            break;
        case func_id::peekchar:
            free_pool_func<PeekChar>(func);
            break;
        case func_id::evallambda:
            free_pool_func<EvalLambda>(func);
            break;
        case func_id::evalquote:
            free_pool_func<EvalQuote>(func);
            break;
        case func_id::evalquoterange:
            free_pool_func<EvalQuoteRange>(func);
            break;
        case func_id::implicitcond:
            free_pool_func<ImplicitCond>(func);
            break;
        case func_id::evalfunc:
            free_pool_func<EvalFunc>(func);
            break;
        case func_id::expandmacro:
            free_pool_func<ExpandMacro>(func);
            break;
        case func_id::evalmacro:
            free_pool_func<EvalMacro>(func);
            break;
        case func_id::empty_id:
        case func_id::end_id:
        default:
            throw logic_error("free_pool_by_funcid: unknown id");
        }
    }
}

#define id_print(elem)\
case elem:\
    os << #elem;\
    break;\

std::ostream& operator<<(std::ostream& os, func_id id)
{
    if (os) {
        switch (id)
        {
            id_print(func_id::prog1)
                id_print(func_id::progn)
                id_print(func_id::eval)
                id_print(func_id::cond)
                id_print(func_id::append)
                id_print(func_id::loop)
                id_print(func_id::apply)
                id_print(func_id::setq)
                id_print(func_id::macroexpand1)
                id_print(func_id::macroexpand)
                id_print(func_id::mapcar)
                id_print(func_id::read)
                id_print(func_id::peekchar)
                id_print(func_id::evallambda)
                id_print(func_id::evalquote)
                id_print(func_id::evalquoterange)
                id_print(func_id::implicitcond)
                id_print(func_id::evalfunc)
                id_print(func_id::expandmacro)
                id_print(func_id::evalmacro)
        case func_id::empty_id:
        case func_id::end_id:
        default:
            throw logic_error("operator<<: unknown id");
        }
    }
    return os;
}


