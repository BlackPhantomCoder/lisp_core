#pragma once
#include "CoreData.h"

#include <optional>
#include <variant>

class CoreEnvironment;

enum class eval_types { eval, no_eval };
enum class spread_types { spread, nospread };

enum class stages : unsigned char { executed, need_external, intermediate, not_args_evaled, not_inited, not_started };

// дефолтный конструктор + дефолтная реализация освобождения через пул
#define default_func(name)\
public:\
	name() = default;\
	virtual void pool_free(Func* ptr) const override{\
		auto ptr2 = (name*)ptr; *ptr2 = name(); CoreData::get_pool<name>().free(ptr2); \
	}


// для правильной работы любой наследник должен иметь дефортный конструктор и оператор копирования 
class Func {
public:
	Func() = default;
	Func(CoreEnvironment& env);
	bool execute();

	void push_next(Cell&& result);
	CoreData::HolderPtr get_next();

	Cell result();

	virtual void pool_free(Func* ptr) const = 0;
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
	CoreEnvironment* t_env_data = nullptr;
	stages t_stage = stages::not_started;
	bool t_eval_next_flag = false;
	bool t_eval_args_flag = false;
};