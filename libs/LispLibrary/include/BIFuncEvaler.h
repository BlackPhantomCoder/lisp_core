#pragma once
#include "Cell.h"
#include "CoreData.h"
#include <stack>
#include <unordered_map>

class CoreEnvironment;

class BifuncEvaler
{
public:
	BifuncEvaler(CoreEnvironment* env);

	void push_bifunc(CoreData::bifunc fnc, Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, bool eval_args);
	void push_nbifunc(CoreData::bifunc fnc, Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it);
	Cell pop_eval();

	void clear();

private:
	CoreEnvironment* t_env;
	struct frame {
		CoreData::bifunc fnc;
		Cell::olist::const_iterator beg_it;
		Cell::olist::const_iterator end_it;
		bool eval_args;
		Cell::olist args;
	};
	std::stack< frame> t_frames;
};

