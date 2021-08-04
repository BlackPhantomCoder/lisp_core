#pragma once
#include <stack>
#include "Cell.h"

class ArgsStack {
public:
	ArgsStack() = default;
	void push(DPair::const_iterator begin, DPair::const_iterator end);
	void pop();
	DPair::const_iterator begin();
	DPair::const_iterator end();
	size_t size() const;
	void clear();
private:
	using stack_type = std::pair<DPair::const_iterator, DPair::const_iterator>;
	std::stack<stack_type, std::vector<stack_type>> t_args;
};
