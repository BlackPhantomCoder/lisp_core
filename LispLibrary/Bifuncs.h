#pragma once
#include "AFuncs.h"
#include "LambdaCell.h"
#include "MacroCell.h"
#include "CoreData.h"

#include <optional>

class Prog1 : public RangeNBiFunc
{
public:
	Prog1(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
	virtual void t_execute_func()  override;
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
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	void t_to_last_f();
private:
	bool t_to_last = true;
	bool t_imp = false;
	Cell t_args_buf;
	CarCdrIterator t_it;
};

class Eval : public RangeBiFunc
{
public:
	Eval(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
	virtual void t_execute_func()  override;
};

class Cond : public RangeNBiFunc
{
public:
	Cond(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	Cell t_args_buf;

	CarCdrIterator t_it;
	bool t_imp = false;
	bool t_ev = false;
	Cell t_buf;
};

class Append : public RangeBiFunc
{
public:
	Append(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	CarCdrIterator t_it;
};

class Loop : public RangeNBiFunc
{
public:
	Loop(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	CarCdrIterator t_it;
	bool t_imp = false;;
};

class Apply : public RangeBiFunc
{
public:
	Apply(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	Cell t_eval_buf;
};

class SetQ : public RangeNBiFunc
{
public:
	SetQ(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	CarCdrIterator t_it;
	bool t_ev = false;
};

class MacroExpand1 : public RangeBiFunc
{
public:
	MacroExpand1(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	bool t_forse_noeval;
};

class MacroExpand : public RangeBiFunc
{
public:
	MacroExpand(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	bool t_forse_noeval;
};

class MapCar : public RangeBiFunc
{
public:
	MapCar(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	std::vector<Cell> t_result;
	Cell t_func;
	std::vector<Cell> t_lists;
	Cell t_buf;
	bool t_finish = false;
	bool t_ev = false;
};