#pragma once
#include "Cell.h"
#include "CoreData.h"
#include <unordered_map>
#include "ArgsOnStackEvaler.h"
class CoreEnvironment;
enum class bifunc_type{bifunc, nbifunc};

struct bifunc_frame {
	bifunc_type type;
	CoreData::bifunc fnc;
	DPair::const_iterator beg_it;
	DPair::const_iterator end_it;
	bool forse_nosread_args;
	DPair args;
};

class BifuncEvaler : public ArgsOnStackEvaler<bifunc_frame, Cell>
{
public:
	BifuncEvaler(CoreEnvironment* env);
	virtual Cell pop_eval() override;
};

