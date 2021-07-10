#pragma once

#include "LambdaCell.h"
#include "Cell.h"
#include <unordered_map>
#include <exception>
#include "Mutexed.h"

struct throwhelper :std::exception{

};

class  CoreEnvironment {
public:
	using CellEnv = std::unordered_map < std::string, Cell>;
public:
	static Cell nil;
	static Cell T;
public:
	CoreEnvironment();

	Cell execute_one(const Cell& c, const Mutexted<bool>& stop_flag);

private:
	Cell bifunc_atomp(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_symbolp(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_numberp(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_listp(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_null(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_add(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_sub(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_mul(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_div(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_greater(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_less(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_less_equal(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_equal(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_car(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_cdr(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_append(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_cons(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_list(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell bifunc_getf(const std::vector<Cell>& c, CellEnv& sub_env);


	Cell nbifunc_quote(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell nbifunc_defun(const std::vector<Cell>& c, CellEnv& sub_env);
	std::pair<bool, Cell>  cond_exec_arc(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell nbifunc_cond(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell nbifunc_if(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell nbifunc_progn(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell nbifunc_setq(const std::vector<Cell>& c, CellEnv& sub_env);
	Cell nbifunc_eval(const std::vector<Cell>& c, CellEnv& sub_env);


	Cell eval(const Cell& arg, CellEnv& sub_env);
	Cell eval_atom(const Cell& atom, CellEnv& sub_env);
	std::pair<bool, Cell> try_bifunc(const std::string& str, const std::vector<Cell>& c, CellEnv& sub_env);
	bool is_bifunc(const std::string& str);
	Cell bool_cast(bool val);
	bool is_T(const Cell& c);
private:
	std::unordered_map < std::string, LambdaCell> t_lambdas;
	std::unordered_map < std::string, Cell> t_vars;
	CellEnv t_env;
	const Mutexted<bool>* t_stop_flag;
};
