#pragma once
#include "Cell.h"
#include "Symbol.h"
#include "json/include/json.hpp"
#include "CoreData.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <map>


class CoreEnvironment;

class CellEnvironment
{
public:
	using mp = std::unordered_map<Symbol, Cell>;
	using frame = std::pair<std::pair<CarCdrIterator, CarCdrIterator>, std::pair<CarCdrIterator, CarCdrIterator>>;

	using stack_elem_type = std::vector<std::pair<Symbol, Cell>, CoreData::allocator<std::pair<Symbol, Cell>>>;
public:
	CellEnvironment(CoreEnvironment& env);

	void init(std::optional<std::reference_wrapper<nlohmann::json>> state);

	void push(const frame& rh);

	void pop();

	size_t size() const;

	const mp& get_globals() const;
	void add_global_var(const Symbol& name, const Cell& val);
	Cell get_global_var(const Symbol& name);

	Cell get(const Symbol& name);
	void set(const Symbol& name, const Cell& val);

	void clear_subenvs();


	// сохрание состояния
	void save_state(nlohmann::json& j);
	// загрузка состояния 
	void load_state(const nlohmann::json& j);
private:
	CoreEnvironment& t_env;
	mp t_glonal;
	
	std::vector<stack_elem_type, CoreData::allocator<stack_elem_type>> t_stack;
	std::unordered_map<Symbol, std::vector<std::pair<size_t, size_t>, CoreData::allocator<std::pair<size_t, size_t>>>> t_all_in_stack;
};

