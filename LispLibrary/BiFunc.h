#pragma once
#include "Func.h"
#include "CoreData.h"
#include "PoolDeleter.h"
#include <optional>

class FuncHolder {
public:
	virtual ~FuncHolder() = default;
	virtual FuncInt& operator()() = 0;
};

template<class T>
class TFuncHolder : public FuncHolder {
public:
	TFuncHolder() = default;

	//template<class... Args>
	//TFuncHolder(Args&&... args) : t_data(T(std::forward<Args&&>(args)...)) {}

	template<class... Args>
	void init(Args&&... args) {
		t_data = T(std::forward<Args&&>(args)...);
	}

	virtual FuncInt& operator()() override{
		if (!t_data) throw "";
		return *t_data;
	}
	void empty() const{
		return !t_data;
	}
	void clear() {
		t_data.reset();
	}
private:
	std::optional<T> t_data;
};

template<class T>
auto& get_pool() {
	static CoreData::ObjPoll<T> p;
	return p;
}

template<class T>
void del_TFuncHolder(FuncHolder* ptr) {
	auto* p = dynamic_cast<TFuncHolder<T>*>(ptr);
	p->clear();
	get_pool<TFuncHolder<T>>().free(p);
}

template<class T, class... Args>
CoreData::HolderPtr make_fnc(Args&&... args) {
	TFuncHolder<T>* obj_ptr = get_pool<TFuncHolder<T>>().get_default();
	obj_ptr->init(std::forward<Args&&>(args)...);
	return std::unique_ptr<TFuncHolder<T>, PollObjDeleter<FuncHolder>>(obj_ptr, PollObjDeleter<FuncHolder>(&del_TFuncHolder<T>));
}

class Func : public FuncInt {
public:
	Func(CoreEnvironment& env);

	virtual stages execute()  final;
	virtual stages stage() const final;

	virtual void push_next(const Cell& result) final;
	virtual FuncInt& get_next() final;

	virtual Cell result() const final;

protected:
	virtual stages t_init_before_args() { return t_cycle(); };
	virtual stages t_init_after_args() { return t_cycle(); };
	virtual bool t_eval_args() = 0;
	virtual stages t_internal_execute() = 0;
protected:
	CoreEnvironment& t_env();
	const CoreEnvironment& t_env() const;

	stages t_return(const Cell& result);
	stages t_return_next(CoreData::HolderPtr&& next);
	stages t_eval_next(CoreData::HolderPtr&& next);
	stages t_cycle();
	Cell t_last_eval_val();
private:
	std::reference_wrapper<CoreEnvironment> t_env_data;
	stages t_stage;
	std::optional<Cell> t_buf;
	CoreData::HolderPtr t_next_func_buf;
	bool t_eval_next_flag = false;
	bool t_eval_args_flag = false;
	bool t_inited = false;
};

class RangeBiFunc : public Func {
public:
	RangeBiFunc(CoreEnvironment& env, CarCdrIterator beg_it, CarCdrIterator end_it, bool forse_noeval = false);
protected:
	virtual stages t_execute_func() = 0;
protected:
	CarCdrIterator t_args_beg();
	CarCdrIterator t_args_end();

	CarCdrConstIterator t_args_beg() const;
	CarCdrConstIterator t_args_end() const;
private:
	virtual bool t_eval_args() override;
	virtual stages t_internal_execute() override;
private:
	std::optional<CoreData::HolderPtr> t_args_eval_func;
	std::variant<Cell, std::pair<CarCdrIterator, CarCdrIterator>, std::monostate> t_args = std::monostate{};
};

class RangeNBiFunc : public Func {
public:
	RangeNBiFunc(CoreEnvironment& env, CarCdrIterator beg_it, CarCdrIterator end_it);
protected:
	virtual stages t_execute_func() = 0;
protected:
	CarCdrIterator t_args_beg();
	CarCdrIterator t_args_end();

	CarCdrConstIterator t_args_beg() const;
	CarCdrConstIterator t_args_end() const;
private:
	virtual bool t_eval_args() override;
	virtual stages t_internal_execute() override;
private:
	std::pair<CarCdrIterator, CarCdrIterator> t_args;
};