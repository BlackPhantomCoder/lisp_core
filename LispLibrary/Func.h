#pragma once
#include "CoreData.h"

#include <optional>
#include <variant>

class CoreEnvironment;

enum class eval_types { eval, no_eval };
enum class spread_types { spread, nospread };

enum class stages : unsigned char { 
	executed,

	before_args_eval,
	need_external_before_args_eval,
	need_external_before_args_eval_plus_next,
	cycle_before_args_eval,

	args_eval,
	need_external_args_eval,
	need_external_args_eval_plus_next,
	cycle_args_eval,

	after_args_eval,
	need_external_after_args_eval,
	need_external_after_args_eval_plus_next,
	cycle_after_args_eval,

	execution,
	need_external_execution,
	cycle_execution,

	need_external_return_next,
};

enum class func_id : unsigned char { 
	empty_id = 0,

	//bifuncs
	prog1,
	progn,
	eval,
	cond,
	append,
	loop,
	apply,
	setq,
	macroexpand1,
	macroexpand,
	mapcar,
	read,
	peekchar,

	//support funcs
	evallambda,
	evalquote,
	evalquoterange,
	implicitcond,
	evalfunc,
	expandmacro,
	evalmacro,

	end_id
};


// дефолтный конструктор + дефолтная реализация освобождения через пул
#define default_func(name)\
public:\
	name() = default;


// для правильной работы любой наследник должен иметь дефортный конструктор и оператор копирования 
class Func {
public:
	Func() = default;
	Func(func_id id, bool skip_eval_args = false);

	stages stage() const;
	func_id id() const;

	// функции управления состояниями

	// вычислить и вернуть результат
	void f_return_next(CoreData::HolderPtr&& next);
	// вычислить (результат будет доступен в s_last_eval_val)
	void f_eval_next(CoreData::HolderPtr&& next);
	// вычислить и перейти (только для before_args_eval,args_eval,after_args_eval) (результат будет доступен в s_last_eval_val)
	void f_eval_next_and_next(CoreData::HolderPtr&& next);
	// ещё одна итерация
	void f_cycle();
	// вернуть результат
	void f_return(const Cell& result);

	// вернуть результат вычисления запрошенной функции
	void f_push_next(const Cell& result);
	//переход 
	void f_next();


	// получение данных

	// результат последнего вычисления f_eval_next (выставленный f_push_next)
	Cell s_last_eval_val();
	// результат, выставленный через f_return (или при вычислении f_return_next)
	Cell s_result();

	// получить последнюю выставленную через f_eval_next/f_return_next функцию
	CoreData::HolderPtr s_get_next();
private:

	std::variant<
		CoreData::HolderPtr,
		Cell,
		std::monostate
	> t_bufs = std::monostate{};
	stages t_stage = stages::before_args_eval;
	func_id t_id = func_id::empty_id;
};

