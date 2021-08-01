#pragma once
#include <string>
#include <unordered_map>
#include <optional>
#include <deque>
#include "Cell.h"

class CellEnvironment
{
public:
	using mp = std::unordered_map<Symbol, Cell>;
public:
	CellEnvironment(mp&& env);
	CellEnvironment(const mp& env = {});

	void push(mp&& rh);
	void push(const mp& rh);
	void pop();

	size_t size() const;

	const mp& get_globals() const;
	void add_global_var(const Symbol& name, const Cell& val);
	std::optional<std::reference_wrapper<Cell>> get_global_var(const Symbol& name);
	std::optional<std::reference_wrapper<const Cell>> get_global_var(const Symbol& name) const;

	std::optional<std::reference_wrapper<Cell>> get(const Symbol& name);
	std::optional<std::reference_wrapper<const Cell>> get(const Symbol& name) const;

	void clear_subenvs();
private:
	mp t_glonal;
	std::deque<mp> t_stack;
};

