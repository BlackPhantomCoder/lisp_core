#pragma once
#include "LambdaCell.h"
#include "CellEnvironment.h"
#include "Cell.h"
#include <stack>
#include "ArgsOnStackEvaler.h"

struct lambda_frame {
	const lambda& fnc;
	DPair::const_iterator beg_it;
	DPair::const_iterator end_it;
	bool forse_nospread_args;
	CellEnvironment::frame buf;
};

class CoreEnvironment;
class LambdaEvaler : public ArgsOnStackEvaler< lambda_frame, Cell>
{
public:
	LambdaEvaler(CoreEnvironment* env);
	virtual Cell pop_eval() override;

	void clear();
private:
	inline void eval_args();
};