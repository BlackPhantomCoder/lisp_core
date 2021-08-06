#pragma once
#include "LambdaCell.h"
#include "CellEnvironment.h"
#include "Cell.h"
#include <stack>
#include <vector>
#include "ArgsOnStackEvaler.h"

struct lambda_frame {
	const lambda& fnc;
	std::vector<Cell>::const_iterator beg_it;
	std::vector<Cell>::const_iterator end_it;
	bool forse_noeval_args;
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