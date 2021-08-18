#pragma once
#include "BiFunc.h"
#include "CellEnvironment.h"

class CoreEnvironment;
class ALambdaFunc : public Func
{
public:
	ALambdaFunc(
		CoreEnvironment& env,
		Cell params,
		Cell body,
		CarCdrIterator beg_it,
		CarCdrIterator end_it,
		eval_types type,
		spread_types stype,
		bool forse_noeval_func
	);
	virtual void t_init_before_args() override;
	virtual void t_init_after_args() override;
	virtual bool t_eval_args() override;
	virtual void t_internal_execute() override;

private:
	CellEnvironment::frame t_create_frame();
private:
	Cell t_params_list;
	Cell t_body;
	Cell t_temp;
	spread_types t_spread_type;

	std::optional<CoreData::HolderPtr> t_args_eval_func;
	std::variant<Cell, std::pair<CarCdrIterator, CarCdrIterator>, std::monostate> t_args = std::monostate{};
};
