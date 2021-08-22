#pragma once
#include "AFuncs.h"
#include "CellEnvironment.h"
#include "LambdaCell.h"
#include "MacroCell.h"

class CoreEnvironment;
class EvalLambda : public Func
{
public:
	EvalLambda(
		CoreEnvironment& env,
		const lambda& l,
		CarCdrIterator beg_it,
		CarCdrIterator end_it,
		bool forse_noeval_args
	);
	virtual void t_init_before_args() override;
	virtual void t_init_after_args() override;
	virtual bool t_eval_args() override;
	virtual void t_internal_execute() override;

private:
	CellEnvironment::frame t_create_frame();
private:
	std::reference_wrapper<const lambda> t_l;
	Cell t_costil_param;
	Cell t_costil_body;
	Cell t_temp;

	std::optional<CoreData::HolderPtr> t_args_eval_func;
	std::variant<Cell, std::pair<CarCdrIterator, CarCdrIterator>, std::monostate> t_args = std::monostate{};
};


class EvalQuote : public Func {
public:
	EvalQuote(CoreEnvironment& env, Cell& c);
private:
	virtual bool t_eval_args() override;
	virtual void t_internal_execute() override;
private:
	void t_eval_func(Cell& fnc, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
private:
	Cell* t_arg;
};

class EvalQuoteRange : public RangeNBiFunc
{
public:
	EvalQuoteRange(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
	virtual void t_init_after_args()  override;
	virtual void t_execute_func()  override;
private:
	void t_to_next();
	void t_eval_fnc(Cell lst);
private:
	std::vector<Cell, CoreData::allocator<Cell>> t_result_v;
	CarCdrIterator t_it;
	bool t_last = false;
};

class ImplicitCond : public Func {
public:
	ImplicitCond(CoreEnvironment& env, Cell& atom);

private:
	virtual void t_init_after_args() override;
	virtual bool t_eval_args() override;
	virtual void t_internal_execute() override;
private:
	Cell* t_atom;
	Cell t_predicate_val_buf;
	Cell t_result;
	bool t_next_res = false;
};


class EvalFunc : public Func
{
public:
	EvalFunc(CoreEnvironment& env, Cell& fnc, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval = false);
private:
	virtual bool t_eval_args() override;
	virtual void t_internal_execute() override;
private:
	Cell* t_fnc;
	CarCdrIterator t_args_beg;
	CarCdrIterator t_args_end;
	CoreData::bifunc t_bifunc = nullptr;
	std::optional<lambda> t_buf;
	bool t_forse_noeval_func;
};



class ExpandMacro : public RangeNBiFunc
{
public:
	ExpandMacro(
		CoreEnvironment& env,
		const macro& macro,
		CarCdrIterator args_beg_it,
		CarCdrIterator args_end_it,
		bool forse_noeval = false,
		bool once = false
	);
private:
	virtual void t_init_after_args()  override;
	virtual void t_execute_func()  override;
private:
	virtual void t_init_from_buf();
private:
	bool t_forse_noeval;
	const macro* t_macro;

	Cell t_param_list;
	CarCdrIterator t_param_list_b;
	CarCdrIterator t_param_list_e;
	Cell t_args;
	Cell t_buf;
	bool t_buf_flag = false;
	bool t_once;
};

class EvalMacro : public Func
{
public:
	EvalMacro(
		CoreEnvironment& env,
		const macro& macro,
		CarCdrIterator args_beg_it,
		CarCdrIterator args_end_it,
		bool forse_noeval = false
	);
private:
	virtual bool t_eval_args() override;
	virtual void t_internal_execute() override;
private:
	CoreData::HolderPtr t_fnc;
	Cell t_buf;
	bool t_ev = false;
};
