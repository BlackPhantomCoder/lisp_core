#pragma once
#include "CoreEnv.h"
#include "CarCdrIterator.h"
#include "Cell.h"

namespace SBIF {
	using iter = CarCdrIterator;

	Cell bifunc_atomp(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_symbolp(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_numberp(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_listp(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_null(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_add(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_sub(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_mul(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_div(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_greater(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_greater_equal(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_less(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_less_equal(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_num_eq(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_equal(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_car(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_cdr(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_cons(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_list(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_getd(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_print(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_prin1(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_set(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_eq(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_last(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_length(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_integerp(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_oblist(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_rplaca(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_rplacd(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_copy_tree(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_pack(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_unpack(CoreEnvironment& env, iter b, iter e);

	Cell bifunc_read_char(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_unread_char(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_listen(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_break(CoreEnvironment& env, iter b, iter e);

	Cell bifunc_get_macro_char(CoreEnvironment& env, iter b, iter e);
	Cell bifunc_set_macro_char(CoreEnvironment& env, iter b, iter e);

	Cell nbifunc_quote(CoreEnvironment& env, iter b, iter e);
	Cell nbifunc_defun(CoreEnvironment& env, iter b, iter e);
	Cell nbifunc_defmacro(CoreEnvironment& env, iter b, iter e);
}