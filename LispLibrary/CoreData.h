#pragma once
#include "boost/pool/pool_alloc.hpp"
#include "MPool.h"
#include "Cell.h"
#include "CarCdrIterator.h"
#include "FuncHolder.h"

#include <array>
#include <exception>

class CoreEnvironment;

namespace CoreData {

	//константы

	inline constexpr const char* nil_str =				"NIL";
	inline constexpr const char* T_str =				"T";
	inline constexpr const char* read_up_case_str =		"*READ-UPCASE*";
	inline constexpr const char* read_base_str =		"*READ-BASE*";

	inline constexpr const char* lambda_str =			"LAMBDA";
	inline constexpr const char* nlambda_str =			"NLAMBDA";
	inline constexpr const char* macro_str =			"MACRO";

	inline constexpr auto bifuncs_count =				39;
	inline constexpr auto nbifuncs_count =				3;

	inline constexpr auto special_bifuncs_count =		8;
	inline constexpr auto special_nbifuncs_count =		5;



	#ifndef TREAT_SAFE_MEMORY_CONTROL
		template<class T>
		using allocator = boost::pool_allocator<T, boost::default_user_allocator_new_delete, boost::details::pool::null_mutex>;

		template<class T>
		using ObjPoll = MPool<T>;



		template<class T>
		inline void allocator_release_memory() {
			boost::singleton_pool<boost::pool_allocator_tag, sizeof(T), boost::default_user_allocator_new_delete, boost::details::pool::null_mutex>::release_memory();
		}
	#endif
	#ifdef TREAT_SAFE_MEMORY_CONTROL
		template<class T>
		using allocator = boost::pool_allocator<T, boost::default_user_allocator_new_delete, boost::details::pool::default_mutex>;
		
		//template<class T>
		//using ObjPoll = DebugPoolWMutex<T>;

		template<class T>
		using ObjPoll = MPoolWMutex<T>;


		template<class T>
		inline void allocator_release_memory() {
			boost::singleton_pool<boost::pool_allocator_tag, sizeof(T), boost::default_user_allocator_new_delete, boost::details::pool::default_mutex>::release_memory();
		}
	#endif

	enum class stream_read_mode { new_string, s_expression };

	struct throw_stop_helper :std::exception {};


	typedef Cell(CoreEnvironment::* bifunc) (CarCdrIterator, CarCdrIterator);


	using bifuncs_array = std::array<std::pair<const char*, bifunc>, bifuncs_count>;
	using nbifuncs_array = std::array<std::pair<const char*, bifunc>, nbifuncs_count>;

	bifuncs_array bifunc_setup();
	nbifuncs_array nbifunc_setup();

	using HolderPtr = FuncHolder;
	template<class T, class... Args>
	CoreData::HolderPtr make_fnc(Args&&... args);

	typedef HolderPtr(*special_bifunc_make) (CoreEnvironment&, CarCdrIterator, CarCdrIterator, bool forse_noeval);
	typedef HolderPtr(*special_nbifunc_make) (CoreEnvironment&, CarCdrIterator, CarCdrIterator);

	using special_bifuncs_array = std::array<std::pair<const char*, special_bifunc_make>, special_bifuncs_count>;
	using special_nbifuncs_array = std::array<std::pair<const char*, special_nbifunc_make>, special_nbifuncs_count>;

	special_bifuncs_array special_bifunc_setup();
	special_nbifuncs_array special_nbifunc_setup();

	extern const bifuncs_array bifuncs_arr;
	extern const nbifuncs_array nbifuncs_arr;
	extern const special_bifuncs_array special_bifuncs_arr;
	extern const special_nbifuncs_array special_nbifuncs_arr;




	struct funcs_pool {
		void push_back(std::function<void(void)>&& func) {
#ifdef TREAT_SAFE_MEMORY_CONTROL
			auto g = std::lock_guard(m);
#endif
			p.push_back(std::move(func));
		}
		void clear() {
#ifdef TREAT_SAFE_MEMORY_CONTROL
			auto g = std::lock_guard(m);
#endif
			for (auto& fnc : p) {
				fnc();
			}
			p.clear();
		}
#ifdef TREAT_SAFE_MEMORY_CONTROL
		std::mutex m;
#endif
		std::vector<std::function<void(void)>> p;
	};

	funcs_pool& clear_pool_funcs();
	void funcs_pools_clear();


	template<class T>
	ObjPoll<T>& get_pool();








	template<class T>
	ObjPoll<T> construct_pool() {
		clear_pool_funcs().push_back([]() { get_pool<T>().clear_free(); });
		return ObjPoll<T>();
	}

	template<class T>
	ObjPoll<T>& get_pool()
	{
		static auto p = construct_pool<T>();
		return p;
	}
	template<class T, class... Args>
	CoreData::HolderPtr make_fnc(Args&&... args) {
		auto* obj_ptr = CoreData::get_pool<T>().construct(std::forward<Args&&>(args)...);
		return FuncHolder(obj_ptr);
	}

}