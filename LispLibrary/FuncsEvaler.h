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
	struct frame {
		frame(CoreData::HolderPtr&& p) : ptr(move(p)), func((*ptr)()){

		}
		CoreData::HolderPtr ptr;
		FuncInt& func;
	};
	std::vector<frame> t_frames;
};

