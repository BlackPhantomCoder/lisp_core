#pragma once
#include "Cell.h"
#include "LambdaCell.h"
#include "CellEnvironment.h"

#include "ArgsOnStackEvaler.h"
#include "CarCdrIterator.h"

struct lambda_frame {
	lambda& fnc;
	CarCdrIterator beg_it;
	CarCdrIterator end_it;
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