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

class  CoreEnvironment : public CoreEnvStreamsProvider {
public:
	typedef Cell(CoreEnvironment::*bifunc) (const std::vector<Cell>&, CellEnvironment&);
	using bifuncs_array = std::array<std::pair<const char*, bifunc>, 35>;
public:
	friend bifuncs_array bifunc_setup();
public:
	static const char* nil_str;
	static const char* T_str;
	static const char* read_up_case_str;

	static const char* lambda_str;
	static const char* nlambda_str;

	static const Cell nil;
	static const Cell T;

	static const bifuncs_array bifuncs;
public:
	CoreEnvironment(std::istream& input, std::ostream& output);

	Cell execute_one(const Cell& c, const IMutexed<bool>& stop_flag);


	const std::unordered_map < std::string, LambdaCell>& lambdas() const;
	const std::unordered_map < std::string, Cell>& vars() const;
private:
	Cell bifunc_atomp(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_symbolp(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_numberp(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_listp(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_null(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_add(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_sub(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_mul(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_div(const std::vector<Cell>& c, CellEnvironment& sub_env);
	//Cell bifunc_mod(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_greater(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_greater_equal(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_less(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_less_equal(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_equal(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_car(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_cdr(const std::vector<Cell>& c, CellEnvironment& sub_env);
	//Cell bifunc_append(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_cons(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_list(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_getd(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_read(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_prog1(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_print(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_set(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_eq(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_last(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_length(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell bifunc_apply(const std::vector<Cell>& c, CellEnvironment& sub_env);

	Cell bifunc_set_modif(Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, CellEnvironment& sub_env);
	Cell bifunc_setq_modif(Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, CellEnvironment& sub_env);

	Cell nbifunc_quote(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell nbifunc_defun(const std::vector<Cell>& c, CellEnvironment& sub_env);

	Cell nbifunc_cond(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell nbifunc_progn(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell nbifunc_progn_modif(Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, CellEnvironment& sub_env);
	Cell nbifunc_setq(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell nbifunc_eval(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell nbifunc_loop(const std::vector<Cell>& c, CellEnvironment& sub_env);
	Cell nbifunc_eq(const std::vector<Cell>& c, CellEnvironment& sub_env);


	Cell eval_quote(const Cell& arg, CellEnvironment& sub_env);
	Cell eval_atom(const Cell& atom, CellEnvironment& sub_env);
	Cell eval_lambda(const LambdaCell& fnc, Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, CellEnvironment& sub_env);
	std::pair<bool, Cell> try_bifunc(const std::string& str, const std::vector<Cell>& c, CellEnvironment& sub_env);
	bool is_bifunc(const std::string& str);
	void eval_rest(Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, CellEnvironment& sub_env);

private:
	std::unordered_map < std::string, LambdaCell> t_lambdas;
	std::unordered_map < std::string, Cell> t_vars;
	CellEnvironment t_env;
	std::optional<std::reference_wrapper<const IMutexed<bool>>> t_stop_flag;
};
