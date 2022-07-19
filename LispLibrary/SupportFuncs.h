#pragma once
#include "Func.h"
#include "RangeNBifunc.h"
#include "CellEnvironment.h"
#include "LambdaCell.h"
#include "MacroCell.h"

class CoreEnvironment;


class EvalLambda : public Func
{
	default_func(EvalLambda)
public:
	EvalLambda(
		const lambda& l,
		CarCdrIterator beg_it,
		CarCdrIterator end_it,
		bool forse_noeval_args
	);
	void execute(CoreEnvironment& env);
private:
	std::variant<
		CoreData::HolderPtr,
		Cell,
		std::pair<CarCdrIterator, CarCdrIterator>,
		std::monostate
	> t_args = std::monostate{};
	lambda_args_types t_arg_type;
	enum class stage : unsigned char { args_eval, args_evaled, main, main_noeval, evaled } t_stage = stage::main;
	
	Cell t_costil_param;
	Cell t_costil_body;
	Cell t_temp;
};


//! c передаётся по ссылке
class EvalQuote : public Func {
	default_func(EvalQuote)
public:
	EvalQuote(Cell& c);
	void execute(CoreEnvironment& env);
private:
	void t_eval_func(Cell& fnc, CarCdrIterator args_beg_it, CarCdrIterator args_end_it);
private:
	Cell* t_arg = nullptr;
};


class EvalQuoteRange : public RangeNBiFunc
{
	simple_rangenbifunc(EvalQuoteRange, func_id::evalquoterange)
	void execute(CoreEnvironment& env);
private:
	void t_push_back_first(CoreEnvironment& env, const Cell& c);
	void t_push_back(CoreEnvironment& env, const Cell& c);
private:
	Cell t_result;
	CarCdrIterator t_it;
	Cell* t_result_last = nullptr;
	enum class stage : unsigned char { start, main, last, func_evaled, func_evaled_first } t_stage = stage::start;
};


class ImplicitCond : public Func {
	default_func(ImplicitCond)
public:
	ImplicitCond(Cell& atom);

	void execute(CoreEnvironment& env);
private:
	Cell* t_atom;
	enum class stage : unsigned char { start, main, res} t_stage = stage::start;
};


class EvalFunc : public Func
{
	default_func(EvalFunc)
public:
	EvalFunc(Cell& fnc, CarCdrIterator args_beg_it, CarCdrIterator args_end_it, bool forse_noeval = false);

	void execute(CoreEnvironment& env);
private:
	std::variant<Cell*, CoreData::bifunc, std::monostate> t_buf;
	bool t_forse_noeval_func;
	CarCdrIterator t_args_beg;
	CarCdrIterator t_args_end;
	
};


class ExpandMacro : public RangeNBiFunc
{
	default_func(ExpandMacro)
public:
	ExpandMacro(
		const macro& macro,
		CarCdrIterator args_beg_it,
		CarCdrIterator args_end_it,
		bool forse_noeval = false,
		bool once = false
	);
	void execute(CoreEnvironment& env);
private:
	const macro* t_macro;
	bool t_forse_noeval;
	enum class stage : unsigned char { once_start, start, main, buf_evaled } t_stage = stage::start;
	Cell t_buf;
	Cell t_param_list;
	CarCdrIterator t_param_list_b;
	CarCdrIterator t_param_list_e;

};


class EvalMacro : public Func
{
	default_func(EvalMacro)
public:
	EvalMacro(
		const macro& macro,
		CarCdrIterator args_beg_it,
		CarCdrIterator args_end_it,
		bool forse_noeval = false
	);
	void execute(CoreEnvironment& env);
private:
	std::variant<CoreData::HolderPtr, Cell, std::monostate> t_buf = std::monostate{};
	enum class stage : unsigned char { start, ev } t_stage = stage::start;
};
