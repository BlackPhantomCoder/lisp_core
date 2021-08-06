#pragma once
#include "Cell.h"
#include "CoreData.h"
#include "ArgsOnStackEvaler.h"
#include <vector>
class CoreEnvironment;
enum class bifunc_type{bifunc, nbifunc};

struct bifunc_frame {
	bifunc_type type;
	CoreData::bifunc fnc;
	std::vector<Cell>::const_iterator beg_it;
	std::vector<Cell>::const_iterator end_it;
	bool forse_nosread_args;
	std::vector<Cell> args;
};

class BifuncEvaler : public ArgsOnStackEvaler<bifunc_frame, Cell>
{
public:
	BifuncEvaler(CoreEnvironment* env);
	virtual Cell pop_eval() override;
};

