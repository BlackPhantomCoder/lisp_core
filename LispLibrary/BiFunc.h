#pragma once
#include "Func.h"
#include "CoreData.h"
#include "PoolDeleter.h"

#include <optional>
#include <variant>

enum class stages { executed, need_external, intermediate, not_args_evaled, not_inited, not_started};

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
	std::reference_wrapper<CoreEnvironment> t_env_data;
	stages t_stage = stages::not_started;
	Cell t_buf;
	CoreData::HolderPtr t_next_func_buf;
	bool t_eval_next_flag = false;
	bool t_eval_args_flag = false;
};

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
	virtual bool t_eval_args() override;
	virtual void t_internal_execute() override;
private:
	std::optional<CoreData::HolderPtr> t_args_eval_func;
	std::variant<Cell, std::pair<CarCdrIterator, CarCdrIterator>, std::monostate> t_args = std::monostate{};
};

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
	virtual bool t_eval_args() override;
	virtual void t_internal_execute() override;
private:
	std::pair<CarCdrIterator, CarCdrIterator> t_args;
};