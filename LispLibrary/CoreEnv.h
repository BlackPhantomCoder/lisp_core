#pragma once
#include "LambdaCell.h"
#include "Mutexed.h"
#include "CoreEnvStreamsProvider.h"
#include "CellEnvironment.h"
#include "CoreData.h"
#include "Lexer.h"
#include "SExprsFarm.h"

#include "FuncsStorage.h"
#include "FuncsEvaler.h"

#include "Cell.h"
#include "Number.h"
#include "Symbol.h"
#include "DotPair.h"
#include "OutputController.h"

#include <unordered_map>
#include <optional>
#include <functional>

struct support_funcs {

	lambda make_spread_lambda_form(
		lambda_types lambda_type,
		const Cell& params,
		Cell& body
	);

	lambda make_nospread_lambda_form(
		lambda_types lambda_type,
		const Symbol& param,
		Cell& body
	);

	macro make_spread_macro_form(
		const Cell& params,
		const Cell& body
	);

	macro make_nospread_macro_form(
		const Symbol& param,
		const Cell& body
	);


	Cell eval_atom(const Cell& atom);
	Cell eval_symbol(const Cell& symbol);
	void set_value(const Cell& name, const Cell& val);
	lambda make_lambda_form(Cell& lst);

	CoreEnvironment* env;
};

class  CoreEnvironment {
	friend struct support_funcs;

	friend CoreData::special_bifuncs_array CoreData::special_bifunc_setup();
	friend CoreData::special_nbifuncs_array CoreData::special_nbifunc_setup();
	friend CoreData::bifuncs_array CoreData::bifunc_setup();
	friend CoreData::nbifuncs_array CoreData::nbifunc_setup();
	friend std::optional<std::reference_wrapper<const macro>> is_macro_call(const Cell& m, CoreEnvironment& e);

	friend class SExprsFarm;
	friend class LambdaEvaler;
	friend class BifuncEvaler;

	friend class FuncsEvaler;

	friend class PtrRangeBiFunc;
	friend class PtrRangeNBiFunc;

	friend class EvalLambda;

	friend class RangeBiFunc;
	friend class RangeNBiFunc;

	friend class Func;
public:
	CoreEnvironment();
	CoreEnvironment(CoreEnvStreamsProvider& streams);
	~CoreEnvironment() = default;

	std::vector<std::string> execute_all(CoreInputStreamInt& stream, const IMutexed<bool>& stop_flag);
	std::string execute_one(CoreInputStreamInt& stream, const IMutexed<bool>& stop_flag);
	const CellEnvironment::mp& vars() const;

	void set_streams(CoreEnvStreamsProvider& streams);
private:
	using iter = CarCdrIterator;
private:
	Cell t_execute(Cell& arg);
	void t_clear_mem();

	Cell bifunc_atomp(iter b, iter e);
	Cell bifunc_symbolp(iter b, iter e);
	Cell bifunc_numberp(iter b, iter e);
	Cell bifunc_listp(iter b, iter e);
	Cell bifunc_null(iter b, iter e);
	Cell bifunc_add(iter b, iter e);
	Cell bifunc_sub(iter b, iter e);
	Cell bifunc_mul(iter b, iter e);
	Cell bifunc_div(iter b, iter e);
	Cell bifunc_greater(iter b, iter e);
	Cell bifunc_greater_equal(iter b, iter e);
	Cell bifunc_less(iter b, iter e);
	Cell bifunc_less_equal(iter b, iter e);
	Cell bifunc_num_eq(iter b, iter e);
	Cell bifunc_equal(iter b, iter e);
	Cell bifunc_car(iter b, iter e);
	Cell bifunc_cdr(iter b, iter e);
	Cell bifunc_cons(iter b, iter e);
	Cell bifunc_list(iter b, iter e);
	Cell bifunc_getd(iter b, iter e);
	Cell bifunc_read(iter b, iter e);
	Cell bifunc_print(iter b, iter e);
	Cell bifunc_prin1(iter b, iter e);
	Cell bifunc_set(iter b, iter e);
	Cell bifunc_eq(iter b, iter e);
	Cell bifunc_last(iter b, iter e);
	Cell bifunc_length(iter b, iter e);
	Cell bifunc_integerp(iter b, iter e);
	Cell bifunc_oblist(iter b, iter e);
	Cell bifunc_rplaca(iter b, iter e);
	Cell bifunc_rplacd(iter b, iter e);
	Cell bifunc_copy_tree(iter b, iter e);
	Cell bifunc_pack(iter b, iter e);
	Cell bifunc_unpack(iter b, iter e);

	Cell nbifunc_quote(iter b, iter e);
	Cell nbifunc_defun(iter b, iter e);
	Cell nbifunc_defmacro(iter b, iter e);

	enum class numbers_compare_type {greater, greater_eq, less, less_eq, eq};
	Cell numbers_compare(iter b, iter e, numbers_compare_type type);

	void t_clear();
private:
	SExprsFarm t_farm;
	std::optional<std::reference_wrapper<CoreEnvStreamsProvider>> t_streams;
	FuncsStorage t_funcs;

	CellEnvironment t_envs;
	std::optional<std::reference_wrapper<const IMutexed<bool>>> t_stop_flag;

	Syntaxer t_syntaxer;
	OutputController t_output_control;

	FuncsEvaler t_funcs_evaler;
	support_funcs t_support;
};
