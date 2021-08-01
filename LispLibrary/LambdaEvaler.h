#pragma once
#include "LambdaCell.h"
#include "Cell.h"
#include <stack>
#include <unordered_map>

class CoreEnvironment;
class LambdaEvaler
{
public:
	LambdaEvaler(CoreEnvironment* env);

	void push_lambda(
		const LambdaCell& fnc, Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, bool forse_eval_args = false
	);
	Cell pop_eval();

	void clear();
private:
	void eval_args();
private:
	CoreEnvironment* t_env;
	struct frame {
		const lambda& fnc;
		lambda_args_types arg_type;
		Cell::olist::const_iterator beg_it;
		Cell::olist::const_iterator end_it;
		bool eval_args;
		std::unordered_map<Symbol, Cell> buf;
	} ;
	std::stack< frame> t_frames;
};

