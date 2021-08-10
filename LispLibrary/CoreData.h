#pragma once
#include "boost/pool/pool_alloc.hpp"
#include "MPool.h"
#include "Cell.h"

#include <array>
#include <exception>

class CoreEnvironment;

namespace CoreData {

	#ifndef TREAT_SAFE_MEMORY_CONTROL
		template<class T>
		using allocator = boost::pool_allocator<T, boost::default_user_allocator_new_delete, boost::details::pool::null_mutex>;

		template<class T>
		using ObjPoll = MPool<T>;
	#endif
	#ifdef TREAT_SAFE_MEMORY_CONTROL
		template<class T>
		using allocator = boost::pool_allocator<T, boost::default_user_allocator_new_delete, boost::details::pool::default_mutex>;
		
		template<class T>
		using ObjPoll = MPoolWMutex<T>;
	#endif

	template<class T>
	void allocator_release_memory();

	enum class stream_read_mode { new_string, s_expression };

	struct throw_stop_helper :std::exception {};
	
	const static char* nil_str = ("NIL");
	const static char* T_str = ("T");
	const static char* read_up_case_str = ("*READ-UPCASE*");

	const static char* lambda_str = ("LAMBDA");
	const static char* nlambda_str = ("NLAMBDA");

	const static auto bifuncs_count = 35;
	const static auto nbifuncs_count = 6;

	typedef Cell(CoreEnvironment::* bifunc) ();

	using bifuncs_array = std::array<std::pair<const char*, bifunc>, bifuncs_count>;
	using nbifuncs_array = std::array<std::pair<const char*, bifunc>, nbifuncs_count>;

	bifuncs_array bifunc_setup();
	nbifuncs_array nbifunc_setup();

	


	struct bifunc_pair_less {
		bool operator()(
			const  std::pair<const char*, CoreData::bifunc>& lh, const  std::pair<const char*, CoreData::bifunc>& rh
		)const;
	};

	struct bifunc_pair_less_w_string {
		bool operator()(const std::pair<const char*, CoreData::bifunc>& lh, const Symbol& rh)const;
	};

}










template<class T>
inline void CoreData::allocator_release_memory() {
	boost::singleton_pool<boost::pool_allocator_tag, sizeof(T)>::release_memory();
}