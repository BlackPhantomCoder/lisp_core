#pragma once
#include <array>
#include <exception>

#include "Cell.h"
class CoreEnvironment;

namespace CoreData {

	enum class stream_read_mode { new_string, s_expression };

	struct throw_stop_helper :std::exception {

	};
	
	const static char* nil_str = ("NIL");
	const static char* T_str = ("T");
	const static char* read_up_case_str = ("*READ-UPCASE*");

	const static char* lambda_str = ("LAMBDA");
	const static char* nlambda_str = ("NLAMBDA");

	const static auto bifuncs_count = 32;
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