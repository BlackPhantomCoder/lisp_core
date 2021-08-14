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
public:
	FuncsEvaler(CoreEnvironment* env);

	Cell eval(Cell& arg);

	void clear();
private:
	CoreEnvironment* t_env;
	//vector invalidate references
	using type = std::reference_wrapper<FuncInt>;
	std::stack< type, std::vector<type>> t_frames;
};

