#pragma once
#include "Cell.h"
#include "Symbol.h"

#include "CoreData.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <map>

class SExprsFarm;

class CellEnvironment
{
public:
	using mp = std::unordered_map<Symbol, Cell>;
	using frame = std::pair<std::pair<CarCdrIterator, CarCdrIterator>, std::pair<CarCdrIterator, CarCdrIterator>>;
public:
	CellEnvironment(SExprsFarm& farm);

	void push(frame&& rh);
	void push(const frame& rh);

	void pop();

	size_t size() const;

	const mp& get_globals() const;
	void add_global_var(const Symbol& name, const Cell& val);
	Cell get_global_var(const Symbol& name);

	Cell get(const Symbol& name);
	void set(const Symbol& name, const Cell& val);

	void clear_subenvs();
private:
	SExprsFarm& t_farm;
	mp t_glonal;
	std::deque<std::pair<frame, std::vector<Cell>>> t_stack;
	std::unordered_map<Symbol, std::vector<Cell*>> t_all_in_stack;
};

