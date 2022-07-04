#pragma once
#include "Func.h"


class RangeBiFunc : public Func {
public:
	RangeBiFunc(CoreEnvironment& env, CarCdrIterator beg_it, CarCdrIterator end_it, bool forse_noeval = false);
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
	std::variant<
		Cell,
		CoreData::HolderPtr,
		std::pair<CarCdrIterator, CarCdrIterator>,
		std::monostate
	> t_args = std::monostate{};
};
