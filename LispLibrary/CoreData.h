#pragma once
#include "boost/pool/pool_alloc.hpp"
#include "MPool.h"
#include "Cell.h"
#include "CarCdrIterator.h"
#include "PoolDeleter.h"

#include <array>
#include <exception>

class CoreEnvironment;
class FuncHolder;

namespace CoreData {

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
		
		template<class T>
		using ObjPoll = MPoolWMutex<T>;


		template<class T>
		inline void allocator_release_memory() {
			boost::singleton_pool<boost::pool_allocator_tag, sizeof(T), boost::default_user_allocator_new_delete, boost::details::pool::default_mutex>::release_memory();
		}
	#endif

	enum class stream_read_mode { new_string, s_expression };

	struct throw_stop_helper :std::exception {};
	
	const static char* const nil_str = "NIL";
	const static char* const T_str = "T";
	const static char* const read_up_case_str = "*READ-UPCASE*";

	const static char* const lambda_str = "LAMBDA";
	const static char* const nlambda_str = "NLAMBDA";
	const static char* const macro_str = "MACRO";

	const static auto bifuncs_count = 41;
	const static auto nbifuncs_count = 3;

	const static auto special_bifuncs_count = 6;
	const static auto special_nbifuncs_count = 5;

	typedef Cell(CoreEnvironment::* bifunc) (CarCdrIterator, CarCdrIterator);


	using bifuncs_array = std::array<std::pair<const char*, bifunc>, bifuncs_count>;
	using nbifuncs_array = std::array<std::pair<const char*, bifunc>, nbifuncs_count>;

	bifuncs_array bifunc_setup();
	nbifuncs_array nbifunc_setup();


	using HolderPtr = std::unique_ptr<FuncHolder, PollObjDeleter<FuncHolder>>;

	typedef HolderPtr(*special_bifunc_make) (CoreEnvironment&, CarCdrIterator, CarCdrIterator, bool forse_noeval);
	typedef HolderPtr(*special_nbifunc_make) (CoreEnvironment&, CarCdrIterator, CarCdrIterator);

	using special_bifuncs_array = std::array<std::pair<const char*, special_bifunc_make>, special_bifuncs_count>;
	using special_nbifuncs_array = std::array<std::pair<const char*, special_nbifunc_make>, special_nbifuncs_count>;

	special_bifuncs_array special_bifunc_setup();
	special_nbifuncs_array special_nbifunc_setup();

	std::vector<std::function<void(void)>>& clear_pool_funcs();
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
}