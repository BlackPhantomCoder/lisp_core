#pragma once

#include "LambdaCell.h"
#include "Cell.h"
#include "Mutexed.h"
#include "CoreEnvStreamsProvider.h"
#include "CellEnvironment.h"
#include <unordered_map>
#include <exception>
#include <array>
#include <optional>
#include <functional>
#include <variant>
#include "LambdaEvaler.h"
#include "CoreData.h"
#include "BIFuncEvaler.h"
#include "SymbolsProvider.h"
#include "Lexer.h"
#include "ArgsStack.h"

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

	friend class LambdaEvaler;
	friend class BifuncEvaler;
public:
	CoreEnvironment(CoreEnvStreamsProvider& streams);
	~CoreEnvironment() = default;

	std::vector<Cell> execute_all(CoreInputStreamInt& stream, const IMutexed<bool>& stop_flag);
	Cell execute_one(CoreInputStreamInt& stream, const IMutexed<bool>& stop_flag);

	const std::unordered_map < Symbol, LambdaCell>& lambdas() const;
	const CellEnvironment::mp& vars() const;
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

	Cell nbifunc_quote();
	Cell nbifunc_defun();
	Cell nbifunc_cond();
	Cell nbifunc_progn();
	Cell nbifunc_setq();
	Cell nbifunc_loop();


	Cell eval_quote(const Cell& atom);
	Cell eval_atom(const Cell& atom);
	std::optional<Cell> eval_implicit_cond(const Cell& atom);
	std::pair<find_bifunc_result, CoreData::bifunc> find_bifunc(const Symbol& str);

	std::vector<Cell> eval_args(Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it);

private:
	SymbolsProvider t_symbols;
	std::unordered_map < Symbol, LambdaCell> t_lambdas;
	ArgsStack t_args;
	CellEnvironment t_envs;
	std::optional<std::reference_wrapper<const IMutexed<bool>>> t_stop_flag;

	LambdaEvaler t_l_evaler;
	BifuncEvaler t_bi_evaler;
	CoreEnvStreamsProvider& t_streams;
	Syntaxer t_syntaxer;
	
	//optimazed buffer for direct call
	Cell t_direct_call_buf;
};
