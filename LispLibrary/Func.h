#pragma once
#include "CoreData.h"
#include "PoolDeleter.h"

#include <optional>
#include <variant>

class CoreEnvironment;

enum class eval_types { eval, no_eval };
enum class spread_types { spread, nospread };

enum class stages : unsigned char { executed, need_external, intermediate, not_args_evaled, not_inited, not_started };

class Func {
public:
	Func(CoreEnvironment& env);
	bool execute();

	void push_next(Cell&& result);
	CoreData::HolderPtr get_next();

	Cell result();

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




class FuncHolder {
public:
	virtual ~FuncHolder() = default;
	virtual Func& operator()() = 0;
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

	virtual Func& operator()() override {
		if (!t_data) throw "";
		return *t_data;
	}
	void empty() const {
		return !t_data;
	}
	void clear() {
		t_data.reset();
	}
private:
	std::optional<T> t_data;
};


template<class T>
void del_TFuncHolder(FuncHolder* ptr) {
	auto* p = (TFuncHolder<T>*)(ptr);
	p->clear();
	CoreData::get_pool<TFuncHolder<T>>().free(p);
}

template<class T, class... Args>
CoreData::HolderPtr make_fnc(Args&&... args) {
	auto* obj_ptr = CoreData::get_pool<TFuncHolder<T>>().get_default();
	obj_ptr->init(std::forward<Args&&>(args)...);
	return std::unique_ptr<TFuncHolder<T>, PollObjDeleter<FuncHolder>>(obj_ptr, PollObjDeleter<FuncHolder>(&del_TFuncHolder<T>));
}
