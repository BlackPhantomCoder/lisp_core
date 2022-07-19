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

#include "common_funcs.h"
constexpr auto free_func = []() {
    using func_type = void (*) (Func*);
    auto result = make_array<func_type, unsigned(func_id::end_id)>([](Func*) {});

    result[(unsigned char)func_id::prog1] = CoreData::pn_pool_free_derived<Prog1>;
    result[(unsigned char)func_id::progn] = CoreData::pn_pool_free_derived<ProgN>;
    result[(unsigned char)func_id::eval] = CoreData::pn_pool_free_derived<Eval>;
    result[(unsigned char)func_id::cond] = CoreData::pn_pool_free_derived<Cond>;
    result[(unsigned char)func_id::append] = CoreData::pn_pool_free_derived<Append>;
    result[(unsigned char)func_id::loop] = CoreData::pn_pool_free_derived<Loop>;
    result[(unsigned char)func_id::apply] = CoreData::pn_pool_free_derived<Apply>;
    result[(unsigned char)func_id::setq] = CoreData::pn_pool_free_derived<SetQ>;
    result[(unsigned char)func_id::macroexpand1] = CoreData::pn_pool_free_derived<MacroExpand1>;
    result[(unsigned char)func_id::macroexpand] = CoreData::pn_pool_free_derived<MacroExpand>;
    result[(unsigned char)func_id::mapcar] = CoreData::pn_pool_free_derived<MapCar>;
    result[(unsigned char)func_id::read] = CoreData::pn_pool_free_derived<Read>;
    result[(unsigned char)func_id::peekchar] = CoreData::pn_pool_free_derived<PeekChar>;


    result[(unsigned char)func_id::evallambda] = CoreData::pn_pool_free_derived<EvalLambda>;
    result[(unsigned char)func_id::evalquote] = CoreData::pn_pool_free_derived<EvalQuote>;
    result[(unsigned char)func_id::evalquoterange] = CoreData::pn_pool_free_derived<EvalQuoteRange>;
    result[(unsigned char)func_id::implicitcond] = CoreData::pn_pool_free_derived<ImplicitCond>;
    result[(unsigned char)func_id::evalfunc] = CoreData::pn_pool_free_derived<EvalFunc>;
    result[(unsigned char)func_id::expandmacro] = CoreData::pn_pool_free_derived<ExpandMacro>;
    result[(unsigned char)func_id::evalmacro] = CoreData::pn_pool_free_derived<EvalMacro>;

    return result;
}();


void free_pool_by_funcid(Func* func)
{
    free_func[unsigned(func->id())](func);
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

