#pragma once
#include "Func.h"

class RangeFunc : public Func {
public:
	RangeFunc() = default;
	RangeFunc(CarCdrIterator beg_it, CarCdrIterator end_it);

	CarCdrConstIterator args_beg() const;
	CarCdrConstIterator args_end() const;
private:
	std::pair<CarCdrIterator, CarCdrIterator> t_args;
};

