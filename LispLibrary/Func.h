#pragma once
#include "FuncInt.h"
#include "CoreData.h"
#include "PoolDeleter.h"
#include "CoreEnv.h"

#include <optional>
#include <variant>

enum class stages : unsigned char { executed, need_external, intermediate, not_args_evaled, not_inited, not_started };

class Func : public FuncInt {
public:
	Func(CoreEnvironment& env);
	virtual bool execute()  final;

	virtual void push_next(Cell&& result) final;
	virtual CoreData::HolderPtr get_next() final;

	virtual Cell result() final;

protected:
	virtual void t_init_before_args();
	virtual void t_init_after_args();
	virtual bool t_eval_args() = 0;
	virtual void t_internal_execute() = 0;
protected:
	CoreEnvironment& t_env();
	const CoreEnvironment& t_env() const;

	void t_return(const Cell& result);
	void t_return_next(CoreData::HolderPtr&& next);
	void t_eval_next(CoreData::HolderPtr&& next);
	void t_cycle();
	Cell t_last_eval_val();
private:

	std::variant<CoreData::HolderPtr, Cell, std::monostate> t_bufs = std::monostate{};
	std::reference_wrapper<CoreEnvironment> t_env_data;
	stages t_stage = stages::not_started;
	bool t_eval_next_flag = false;
	bool t_eval_args_flag = false;
};
