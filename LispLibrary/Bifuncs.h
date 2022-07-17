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
	simple_rangenbifunc(Prog1, func_id::prog1)
	void execute(CoreEnvironment& env);
private:
	enum class stage : unsigned char {start, progn, imp, ev } t_stage = stage::start;
	Cell t_result_buf;
};

class ProgN : public RangeNBiFunc
{
	simple_rangenbifunc(ProgN, func_id::progn)
	void execute(CoreEnvironment& env);
private:
	enum class stage : unsigned char { start, last_imp, eval_imp, eval, last } t_stage = stage::start;
	Cell t_args_buf;
	CarCdrIterator t_it;
};

class Eval : public RangeBiFunc
{
	default_func(Eval)
public:
	Eval(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
	void execute(CoreEnvironment& env);
};


class Cond : public RangeNBiFunc
{
	simple_rangenbifunc(Cond, func_id::cond)
	void execute(CoreEnvironment& env);
private:
	enum class stage : unsigned char { start, main, imp, ev } t_stage = stage::start;
	Cell t_args_buf;
	CarCdrIterator t_it;
	Cell t_buf;
};

class Append : public RangeBiFunc
{
	default_func(Append)
public:
	Append(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
	void after_args_eval(CoreEnvironment& env);
	void execute(CoreEnvironment& env);
private:
	CarCdrIterator t_it;
};

class Loop : public RangeNBiFunc
{
	simple_rangenbifunc(Loop, func_id::loop)
	void execute(CoreEnvironment& env);
private:
	enum class stage : unsigned char { start, main, imp } t_stage = stage::start;
	CarCdrIterator t_it;
};

class Apply : public RangeBiFunc
{
	default_func(Apply)
public:
	Apply(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval);
	void after_args_eval(CoreEnvironment& env);
	void execute(CoreEnvironment& env);
private:
	Cell t_eval_buf;
};

class SetQ : public RangeNBiFunc
{
	simple_rangenbifunc(SetQ, func_id::setq)
	void execute(CoreEnvironment& env);
private:
	enum class stage : unsigned char { start, repeat_start, main, ev } t_stage = stage::start;
	CarCdrIterator t_it;
};

class MacroExpand1 : public RangeBiFunc
{
	default_func(MacroExpand1)
public:
	MacroExpand1(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval);
	void after_args_eval(CoreEnvironment& env);
	void execute(CoreEnvironment& env);
private:
	bool t_forse_noeval = false;
};

class MacroExpand : public RangeBiFunc
{
	default_func(MacroExpand)
public:
	MacroExpand(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval);
	void after_args_eval(CoreEnvironment& env);
	void execute(CoreEnvironment& env);
private:
	bool t_forse_noeval = false;
};

class MapCar : public RangeBiFunc
{
	default_func(MapCar)
public:
	MapCar(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval);
	void after_args_eval(CoreEnvironment& env);
	void execute(CoreEnvironment& env);
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
	Read(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
	void before_args_eval(CoreEnvironment& env);
	void execute(CoreEnvironment& env);
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
	PeekChar(CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval_func = false);
	void after_args_eval(CoreEnvironment& env);
	void execute(CoreEnvironment& env);
private:
	int t_i = 0;
	bool t_skip_comments = false;
	bool t_until = false;
	char t_until_char = 0;

	bool t_skip_comments_cycle = false;
	bool t_until_char_cycle = false;	
};

