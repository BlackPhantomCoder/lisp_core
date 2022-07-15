#pragma once
#include "Func.h"
#include "RangeNBifunc.h"
#include "RangeBifunc.h"
#include "LambdaCell.h"
#include "MacroCell.h"
#include "CoreData.h"
#include "parser.h"
#include "ParserHelper.h"

#include <optional>

class Prog1 : public RangeNBiFunc
{
	default_func(Prog1)
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
	default_func(ProgN)
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
	default_func(Eval)
public:
	Eval(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
	virtual void t_execute_func()  override;
};



class Cond : public RangeNBiFunc
{
	default_func(Cond)
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
	default_func(Append)
public:
	Append(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	CarCdrIterator t_it;
};

class Loop : public RangeNBiFunc
{
	default_func(Loop)
public:
	Loop(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	CarCdrIterator t_it;
	bool t_imp = false;
};

class Apply : public RangeBiFunc
{
	default_func(Apply)
public:
	Apply(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	Cell t_eval_buf;
};

class SetQ : public RangeNBiFunc
{
	default_func(SetQ)
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
	default_func(MacroExpand1)
public:
	MacroExpand1(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	bool t_forse_noeval = false;
};

class MacroExpand : public RangeBiFunc
{
	default_func(MacroExpand)
public:
	MacroExpand(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	bool t_forse_noeval = false;
};

class MapCar : public RangeBiFunc
{
	default_func(MapCar)
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


class Read : public RangeBiFunc
{
	default_func(Read)
public:
	Read(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
	virtual void t_execute_func()  override;
private:
	ParserHelper t_hp;
	yypstate* t_parser = nullptr;
	Cell t_buf;
	bool t_macro = false;
};

class PeekChar : public RangeBiFunc
{
	default_func(PeekChar)
public:
	PeekChar(CoreEnvironment& env, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
	virtual void t_init_after_args() override;
	virtual void t_execute_func()  override;
private:
	int t_i = 0;
	bool t_skip_comments = false;
	bool t_until = false;
	char t_until_char = 0;

	bool t_skip_comments_cycle = false;
	bool t_until_char_cycle = false;	
};

