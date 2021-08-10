#pragma once
#include "Cell.h"
#include "CoreData.h"
#include "ArgsOnStackEvaler.h"
#include "CarCdrIterator.h"

#include <vector>

class CoreEnvironment;
enum class bifunc_type : bool{bifunc, nbifunc};

struct bifunc_frame {
	bifunc_type type;
	CoreData::bifunc fnc;
	CarCdrIterator beg_it;
	CarCdrIterator end_it;
	bool forse_nosread_args;
	Cell args;
};

class BifuncEvaler : public ArgsOnStackEvaler<bifunc_frame, Cell>
{
public:
	BifuncEvaler(CoreEnvironment* env);
	virtual Cell pop_eval() override;
};

