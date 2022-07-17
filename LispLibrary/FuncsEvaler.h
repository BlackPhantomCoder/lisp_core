#pragma once
#include "Func.h"
#include "Cell.h"
#include "CellEnvironment.h"
#include "CoreData.h"

#include <memory>
#include <stack>
#include <deque>

class CoreEnvironment;
class FuncsEvaler
{
	enum class execute_result {result, external, check};
public:
	FuncsEvaler(CoreEnvironment* env);

	//Cell eval(Cell& arg);
	Cell eval(CoreData::HolderPtr&& func);

	void clear();
private:
	execute_result t_func_execute(Func& func);
private:
	CoreEnvironment* t_env;
	std::vector<CoreData::HolderPtr> t_funcs;
};

