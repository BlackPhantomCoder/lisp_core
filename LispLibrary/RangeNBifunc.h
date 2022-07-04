#pragma once
#include "Func.h"

class RangeNBiFunc : public Func {
public:
	RangeNBiFunc(CoreEnvironment& env, CarCdrIterator beg_it, CarCdrIterator end_it);
protected:
	virtual void t_execute_func() = 0;
protected:
	CarCdrIterator t_args_beg();
	CarCdrIterator t_args_end();

	CarCdrConstIterator t_args_beg() const;
	CarCdrConstIterator t_args_end() const;
private:
	virtual bool t_eval_args() final;
	virtual void t_internal_execute() final;
private:
	std::pair<CarCdrIterator, CarCdrIterator> t_args;
};

