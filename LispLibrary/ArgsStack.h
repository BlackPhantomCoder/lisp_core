#pragma once
#include <stack>
#include <vector>
#include "Cell.h"
#include "DotPair.h"

class ArgsStack {
public:
	ArgsStack() = default;
	void push(std::vector<Cell>::const_iterator begin, std::vector<Cell>::const_iterator end);
	void pop();
	std::vector<Cell>::const_iterator begin();
	std::vector<Cell>::const_iterator end();
	size_t size() const;
	void clear();
private:
	using stack_type = std::pair<std::vector<Cell>::const_iterator, std::vector<Cell>::const_iterator>;
	std::stack<stack_type, std::vector<stack_type>> t_args;
};
