#pragma once
#include <stack>
#include "Cell.h"

class ArgsStack {
public:
	ArgsStack() = default;
	void push(Cell::olist::const_iterator begin, Cell::olist::const_iterator end);
	void pop();
	Cell::olist::const_iterator begin();
	Cell::olist::const_iterator end();
	size_t size() const;
	void clear();
	std::stack<std::pair<Cell::olist::const_iterator, Cell::olist::const_iterator>> t_args;
};
