#pragma once
#include "CoreData.h"
#include "Func.h"
#include "Bifuncs.h"
#include "SupportFuncs.h"

#include <array>
#include <type_traits>
#include <functional>



void free_pool_by_funcid(Func* func);

std::ostream& operator<<(std::ostream& os, func_id id);


class CoreEnvironment;

struct func_data {

	using caller_type = void(*)(void*, CoreEnvironment&);
	enum class core_type { undefined, func, range_bifunc, range_nbifunc };

	constexpr func_data() = default;

	core_type core_type = core_type::undefined;
	caller_type func_before_args = nullptr;
	caller_type func_execute = nullptr;
	caller_type func_after_args = nullptr;
};


template<class T, bool func_before_args, bool func_after_args>
constexpr func_data make_func_data();

template<class T>
constexpr func_data make_range_nbifunc_data();


template <class T>
void caller_before_args(void* obj, CoreEnvironment& env);

template <class T>
void caller_execute(void* obj, CoreEnvironment& env);

template <class T>
void caller_after_args(void* obj, CoreEnvironment& env);


extern const std::array<func_data, (unsigned char)func_id::end_id> func_table;









template <class T>
void caller_before_args(void* obj, CoreEnvironment& env) {
	((T*)obj)->before_args_eval(env);
}

template <class T>
void caller_execute(void* obj, CoreEnvironment& env) {
	((T*)obj)->execute(env);
}

template <class T>
void caller_after_args(void* obj, CoreEnvironment& env) {
	((T*)obj)->after_args_eval(env);
}

template<class T, bool func_before_args, bool func_after_args>
constexpr func_data make_func_data()
{

	auto data = func_data{};
	data.core_type = func_data::core_type::undefined;

	if constexpr(func_before_args)
		data.func_before_args = caller_before_args<T>;
	data.func_execute = caller_execute<T>;
	if constexpr (func_after_args)
		data.func_after_args = caller_after_args<T>;


	if constexpr (std::is_base_of_v<RangeNBiFunc, T>) {
		data.core_type = func_data::core_type::range_nbifunc;
	}
	else if constexpr (std::is_base_of_v<RangeBiFunc, T>) {
		data.core_type = func_data::core_type::range_bifunc;
	}
	else {
		data.core_type = func_data::core_type::func;
	}
	return data;
}

template<class T>
constexpr func_data make_range_nbifunc_data()
{
	return make_func_data<T, false, false>();
}
