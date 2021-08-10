#pragma once
#include "LambdaCell.h"
#include "Mutexed.h"
#include "CoreEnvStreamsProvider.h"
#include "CellEnvironment.h"
#include "LambdaEvaler.h"
#include "CoreData.h"
#include "BIFuncEvaler.h"
#include "Lexer.h"
#include "ArgsStack.h"
#include "SExprsFarm.h"

#include "FuncsStorage.h"

#include "Cell.h"
#include "Number.h"
#include "Symbol.h"
#include "DotPair.h"

#include <unordered_map>
#include <optional>
#include <functional>

#define eval_direct_bifunc(f, b, e)\
    (\
    t_args.push(b, e), \
    t_direct_call_buf = (this->*(f))(), \
    t_args.pop(), \
    t_direct_call_buf\
    )

#define env_eval_direct_bifunc(f, b, e, env_ptr)\
    (\
    env_ptr->t_args.push(b, e), \
    env_ptr->t_direct_call_buf = (env_ptr->*(f))(), \
    env_ptr->t_args.pop(), \
    env_ptr->t_direct_call_buf\
    )

class  CoreEnvironment {
	friend CoreData::bifuncs_array CoreData::bifunc_setup();
	friend CoreData::nbifuncs_array CoreData::nbifunc_setup();

	friend class SExprsFarm;
	friend class LambdaEvaler;
	friend class BifuncEvaler;
public:
	CoreEnvironment();
	CoreEnvironment(CoreEnvStreamsProvider& streams);
	~CoreEnvironment() = default;

	std::vector<std::string> execute_all(CoreInputStreamInt& stream, const IMutexed<bool>& stop_flag);
	std::string execute_one(CoreInputStreamInt& stream, const IMutexed<bool>& stop_flag);
	const CellEnvironment::mp& vars() const;

	void set_streams(CoreEnvStreamsProvider& streams);
private:
	enum class find_bifunc_result {bi, nbi, null};
private:
	Cell bifunc_atomp();
	Cell bifunc_symbolp();
	Cell bifunc_numberp();
	Cell bifunc_listp();
	Cell bifunc_null();
	Cell bifunc_add();
	Cell bifunc_sub();
	Cell bifunc_mul();
	Cell bifunc_div();
	//Cell bifunc_mod();
	Cell bifunc_greater();
	Cell bifunc_greater_equal();
	Cell bifunc_less();
	Cell bifunc_less_equal();
	Cell bifunc_num_eq();
	Cell bifunc_equal();
	Cell bifunc_car();
	Cell bifunc_cdr();
	Cell bifunc_append();
	Cell bifunc_cons();
	Cell bifunc_list();
	Cell bifunc_getd();
	Cell bifunc_read();
	Cell bifunc_prog1();
	Cell bifunc_print();
	Cell bifunc_set();
	Cell bifunc_eq();
	Cell bifunc_last();
	Cell bifunc_length();
	Cell bifunc_apply();
	Cell bifunc_eval();
	Cell bifunc_integerp();
	Cell bifunc_oblist();
	Cell bifunc_rplaca();
	Cell bifunc_rplacd();
	Cell bifunc_copy_tree();

	Cell nbifunc_quote();
	Cell nbifunc_defun();
	Cell nbifunc_cond();
	Cell nbifunc_progn();
	Cell nbifunc_setq();
	Cell nbifunc_loop();

	Cell eval_quote(Cell& arg);

	lambda make_spread_lambda_form(
		lambda_types lambda_type,
		Cell& params,
		Cell& body
	);

	lambda make_nospread_lambda_form(
		lambda_types lambda_type,
		const Symbol& param,
		Cell& body
	);

	Cell eval_atom(const Cell& atom);
	Cell eval_symbol(const Cell& symbol);
	std::optional<Cell> eval_implicit_cond(Cell& atom);
	void set_value(const Cell& name, const Cell& val);
	Cell eval_fnc(
		Cell& fnc,
		CarCdrIterator args_beg_it,
		CarCdrIterator args_end_it,
		bool forse_nospread_args
	);
	lambda get_lambda_form(Cell& lst);

	enum class numbers_compare_type {greater, greater_eq, less, less_eq, eq};
	Cell numbers_compare(numbers_compare_type type);
private:
	SExprsFarm t_farm;
	std::optional<std::reference_wrapper<CoreEnvStreamsProvider>> t_streams;
	FuncsStorage t_funcs;

	ArgsStack t_args;
	CellEnvironment t_envs;
	std::optional<std::reference_wrapper<const IMutexed<bool>>> t_stop_flag;

	LambdaEvaler t_l_evaler;
	BifuncEvaler t_bi_evaler;
	Syntaxer t_syntaxer;
	
	//optimazed buffer for direct call
	Cell t_direct_call_buf;
	std::pair<bool, std::pair<bifunc_type, CoreData::bifunc>> t_findbifunc_buf;
};
