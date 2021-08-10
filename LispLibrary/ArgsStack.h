#pragma once
#include <stack>
#include <vector>
#include "Cell.h"
#include "DotPair.h"
#include "CarCdrIterator.h"


class ArgsStack {
public:
	using iterator = CarCdrIterator;
public:
	ArgsStack() = default;
	void push(iterator begin, iterator end);
	void pop();
	iterator begin();
	iterator end();

	bool operator==(long val) const;
	bool operator>=(long val) const;
	bool operator<=(long val) const;
	bool operator>(long val) const;
	bool operator<(long val) const;
	void clear();
private:
	using stack_type = std::pair<iterator, iterator>;
	std::stack<stack_type, std::vector<stack_type>> t_args;
};
