#pragma once
#include "Cell.h"
#include "Symbol.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <map>

class CellEnvironment
{
public:
	using mp = std::unordered_map<Symbol, Cell>;
	using frame = std::vector<std::pair<Symbol, Cell>>;
public:
	CellEnvironment(mp&& env);
	CellEnvironment(const mp& env = {});

	void push(frame&& rh);
	void push(const frame& rh);

	void pop();

	size_t size() const;

	const mp& get_globals() const;
	void add_global_var(const Symbol& name, const Cell& val);
	Cell* get_global_var(const Symbol& name);
	const Cell* get_global_var(const Symbol& name) const;

	Cell* get(const Symbol& name);
	const Cell* get(const Symbol& name) const;

	void clear_subenvs();
private:
	mp t_glonal;
	std::vector<frame> t_stack;
	std::unordered_map<Symbol, std::vector<Cell*>> t_all_in_stack;
};

