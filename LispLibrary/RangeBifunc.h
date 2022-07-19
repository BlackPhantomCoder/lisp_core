#pragma once
#include "Func.h"

class RangeBiFunc : public Func {
	friend class CoreEnvironment;
public:
	RangeBiFunc() = default;
	RangeBiFunc(func_id id, CarCdrIterator beg_it, CarCdrIterator end_it, bool forse_noeval = false);

	CarCdrIterator args_beg();
	CarCdrIterator args_end();

	CarCdrConstIterator args_beg() const;
	CarCdrConstIterator args_end() const;

private:
	void t_eval_args();
private:
	std::variant<
		Cell,
		CoreData::HolderPtr,
		std::pair<CarCdrIterator, CarCdrIterator>,
		std::monostate
	> t_args = std::monostate{};
};

