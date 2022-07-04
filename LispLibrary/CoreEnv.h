#pragma once
#include "LambdaCell.h"
#include "Mutexed.h"
#include "CoreEnvStreamsProvider.h"
#include "CellEnvironment.h"
#include "CoreData.h"
#include "Input.h"
#include "Input/MacroTable.h"
#include "Input/Scanner.h"
#include "SExprsFarm.h"

#include "FuncsStorage.h"
#include "FuncsEvaler.h"

#include "Cell.h"
#include "Number.h"
#include "Symbol.h"
#include "DotPair.h"
#include "OutputController.h"
#include "CellSerializer.h"
#include "json/include/json.hpp"

#include <unordered_map>
#include <optional>
#include <functional>

struct break_helper {
	std::string s;
};

class CoreEnvironment;

struct support_funcs {

	lambda make_spread_lambda_form(
		lambda_types lambda_type,
		const Cell& params,
		Cell& body
	);

	lambda make_nospread_lambda_form(
		lambda_types lambda_type,
		const Symbol& param,
		Cell& body
	);

	macro make_spread_macro_form(
		const Cell& params,
		const Cell& body
	);

	macro make_nospread_macro_form(
		const Symbol& param,
		const Cell& body
	);


	Cell eval_atom(const Cell& atom);
	Cell eval_symbol(const Cell& symbol);
	void set_value(const Cell& name, const Cell& val);
	lambda make_lambda_form(Cell& lst);

	CoreEnvironment* env;
};

class  CoreEnvironment {
	friend class Core;
	using iter = CarCdrIterator;
public:
	CoreEnvironment(std::optional<std::reference_wrapper<std::istream>> state);
	CoreEnvironment(CoreEnvStreamsProviderInt& streams, std::optional<std::reference_wrapper<std::istream>> state);
	~CoreEnvironment() = default;

	// сохрание состояния в поток
	// (не полного)
	void save_state(std::ostream& os);
	// загрузка из состояния
	// (очищает текущее)
	bool load_state(std::istream& is);

	void execute_all(CoreEnvStreamsProviderInt& streams, const IMutexed<bool>& stop_flag);
	void execute_one(CoreEnvStreamsProviderInt& streams, const IMutexed<bool>& stop_flag);
	void execute_driver(CoreEnvStreamsProviderInt& streams, const IMutexed<bool>& stop_flag);

	void set_streams(CoreEnvStreamsProviderInt& streams);


	Cell bifunc_atomp(iter b, iter e);
	Cell bifunc_symbolp(iter b, iter e);
	Cell bifunc_numberp(iter b, iter e);
	Cell bifunc_listp(iter b, iter e);
	Cell bifunc_null(iter b, iter e);
	Cell bifunc_add(iter b, iter e);
	Cell bifunc_sub(iter b, iter e);
	Cell bifunc_mul(iter b, iter e);
	Cell bifunc_div(iter b, iter e);
	Cell bifunc_greater(iter b, iter e);
	Cell bifunc_greater_equal(iter b, iter e);
	Cell bifunc_less(iter b, iter e);
	Cell bifunc_less_equal(iter b, iter e);
	Cell bifunc_num_eq(iter b, iter e);
	Cell bifunc_equal(iter b, iter e);
	Cell bifunc_car(iter b, iter e);
	Cell bifunc_cdr(iter b, iter e);
	Cell bifunc_cons(iter b, iter e);
	Cell bifunc_list(iter b, iter e);
	Cell bifunc_getd(iter b, iter e);
	Cell bifunc_print(iter b, iter e);
	Cell bifunc_prin1(iter b, iter e);
	Cell bifunc_set(iter b, iter e);
	Cell bifunc_eq(iter b, iter e);
	Cell bifunc_last(iter b, iter e);
	Cell bifunc_length(iter b, iter e);
	Cell bifunc_integerp(iter b, iter e);
	Cell bifunc_oblist(iter b, iter e);
	Cell bifunc_rplaca(iter b, iter e);
	Cell bifunc_rplacd(iter b, iter e);
	Cell bifunc_copy_tree(iter b, iter e);
	Cell bifunc_pack(iter b, iter e);
	Cell bifunc_unpack(iter b, iter e);

	Cell bifunc_read_char(iter b, iter e);
	Cell bifunc_unread_char(iter b, iter e);
	Cell bifunc_listen(iter b, iter e);
	Cell bifunc_break(iter b, iter e);

	Cell bifunc_get_macro_char(iter b, iter e);
	Cell bifunc_set_macro_char(iter b, iter e);

	Cell nbifunc_quote(iter b, iter e);
	Cell nbifunc_defun(iter b, iter e);
	Cell nbifunc_defmacro(iter b, iter e);



	SExprsFarm& farm();
	std::optional<std::reference_wrapper<const IMutexed<bool>>>& stop_flag();
	std::optional<std::reference_wrapper<CoreOutputStreamInt>>& cos();
	support_funcs& support_funcs();
	OutputController& output_control();
	CellEnvironment& envs();
	MacroTable& macrotable();
	Input& input();
	Scanner& scanner();
	FuncsStorage& funcs();
	CellSerializer& cserial();
private:

	enum class numbers_compare_type { greater, greater_eq, less, less_eq, eq };
	Cell t_numbers_compare(iter b, iter e, numbers_compare_type type);

	template <class Fnc>
	void t_core_env_under_catch(Fnc fnc);
	void t_clear();
	void t_prepare(CoreEnvStreamsProviderInt& streams, const IMutexed<bool>& stop_flag);
	void t_execute();
	Cell t_execute(Cell& arg);
	Cell t_execute(CoreData::HolderPtr&& func);
	void t_clear_mem();
private:
	SExprsFarm t_farm;
	FuncsStorage t_funcs;
	CellEnvironment t_envs;
	MacroTable t_macrotable;

	Input t_input;
	Scanner t_scanner;
	OutputController t_output_control;
	::support_funcs t_support;
	CellSerializer t_cserial;
	//FuncsEvaler t_funcs_evaler;

	std::optional<std::reference_wrapper<const IMutexed<bool>>> t_stop_flag;
	std::optional<std::reference_wrapper<CoreEnvStreamsProviderInt>> t_streams;
	std::optional<std::reference_wrapper<CoreOutputStreamInt>> t_cos;
};

template<class Fnc>
inline void CoreEnvironment::t_core_env_under_catch(Fnc fnc) {
#ifndef EX_CATCH
	fnc();
#endif

#ifdef EX_CATCH
	try
	{
		fnc();
	}
	catch (std::bad_alloc&) {
		this->t_clear();
		throw "stack overflow";
	}
	catch (break_helper& e) {
		this->t_clear();
		throw "break: " + e.s;
	}
	catch (...)
	{
		this->t_clear();
		throw;
	}
#endif
}
