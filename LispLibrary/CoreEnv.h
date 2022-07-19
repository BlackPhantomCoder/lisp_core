#pragma once
#include "LambdaCell.h"
#include "Cell.h"
#include "Number.h"
#include "Symbol.h"
#include "DotPair.h"
#include "Mutexed.h"
#include "CoreEnvStreamsProvider.h"
#include "CoreData.h"

#include "CellEnvironment.h"
#include "Input.h"
#include "Input/MacroTable.h"
#include "Input/Scanner.h"
#include "SExprsFarm.h"
#include "CoreFuncs.h"
#include "FuncsStorage.h"
#include "OutputController.h"
#include "CellSerializer.h"

#include "json/include/json.hpp"

#include <optional>

struct break_helper {
	std::string s;
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


	SExprsFarm& farm();
	std::optional<std::reference_wrapper<const IMutexed<bool>>>& stop_flag();
	std::optional<std::reference_wrapper<CoreOutputStreamInt>>& cos();
	CoreFuncs& core_funcs();
	OutputController& output_control();
	CellEnvironment& envs();
	MacroTable& macrotable();
	Input& input();
	Scanner& scanner();
	FuncsStorage& funcs();
	CellSerializer& cserial();
private:

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
	CoreFuncs t_corefuncs;
	CellSerializer t_cserial;

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
