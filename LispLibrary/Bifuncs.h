#pragma once
#include "BiFunc.h"
#include "LambdaCell.h"

#include <optional>
#include "CoreData.h"

class EvalQuote : public Func {
public:
	EvalQuote(CoreEnvironment& env, Cell& c);

private:
	virtual bool t_eval_args() override;
	virtual stages t_internal_execute() override;
private:
	stages t_eval_func(Cell& fnc, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
private:
	Cell t_arg;
};

class EvalQuoteRange : public RangeNBiFunc
{
public:
	EvalQuoteRange(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
	virtual stages t_init_after_args()  override;
	virtual stages t_execute_func()  override;
private:
	stages t_to_next();
	stages t_eval_fnc(Cell lst);
private:
	std::vector<Cell, CoreData::allocator<Cell>> t_result_v;
	CarCdrIterator t_it;
	bool t_last = false;
};

class Prog1 : public RangeNBiFunc
{
public:
	Prog1(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
	virtual stages t_execute_func()  override;
private:
	bool t_progn = false;
	bool t_imp = false;
	bool t_ev = false;
	Cell t_result_buf;
};


class ProgN : public RangeNBiFunc
{
public:
	ProgN(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
	virtual stages t_init_after_args() override;
	virtual stages t_execute_func()  override;
private:
	stages t_to_last_f();
private:
	bool t_to_last = true;
	bool t_imp = false;
	Cell t_args_buf;
	CarCdrIterator t_it;
};

class ImplicitCond : public Func {
public:
	ImplicitCond(CoreEnvironment& env, Cell& atom);

private:
	virtual stages t_init_after_args() override;
	virtual bool t_eval_args() override;
	virtual stages t_internal_execute() override;
private:
	Cell t_atom;
	std::optional<Cell> t_predicate_val_buf;
	std::optional<Cell> t_result;
};

class Eval : public RangeBiFunc
{
public:
	Eval(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
	virtual stages t_execute_func()  override;
};

class Cond : public RangeNBiFunc
{
public:
	Cond(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
	virtual stages t_init_after_args() override;
	virtual stages t_execute_func()  override;
private:
	Cell t_args_buf;

	CarCdrIterator t_it;
	bool t_imp = false;
};


class EvalFunc : public Func
{
public:
	EvalFunc(CoreEnvironment& env, Cell& fnc, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
private:
	virtual bool t_eval_args() override;
	virtual stages t_internal_execute() override;
private:
	Cell t_fnc;
	CarCdrIterator t_args_beg;
	CarCdrIterator t_args_end;
	CoreData::bifunc t_bifunc = nullptr;
	bool t_forse_noeval_func;
};

class Append : public RangeBiFunc
{
public:
	Append(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
	virtual stages t_init_after_args() override;
	virtual stages t_execute_func()  override;
private:
	CarCdrIterator t_it;
};

class Loop : public RangeNBiFunc
{
public:
	Loop(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
	virtual stages t_init_after_args() override;
	virtual stages t_execute_func()  override;
private:
	CarCdrIterator t_it;
	bool t_imp = false;;
};

class Apply : public RangeBiFunc
{
public:
	Apply(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval);
	virtual stages t_init_after_args() override;
	virtual stages t_execute_func()  override;
private:
	Cell t_eval_buf;
};

class SetQ : public RangeNBiFunc
{
public:
	SetQ(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
	virtual stages t_init_after_args() override;
	virtual stages t_execute_func()  override;
private:
	CarCdrIterator t_it;
	bool t_ev = false;
};