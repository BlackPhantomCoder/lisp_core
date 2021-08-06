#include "ArgsStack.h"

void ArgsStack::push(std::vector<Cell>::const_iterator begin, std::vector<Cell>::const_iterator end)
{
	t_args.push({ begin, end });
}

void ArgsStack::pop() {
	if (empty(t_args)) throw "args stack empty";
	t_args.pop();
}

std::vector<Cell>::const_iterator ArgsStack::begin() {
	if (empty(t_args)) throw "args stack empty";
	return t_args.top().first;
}

std::vector<Cell>::const_iterator ArgsStack::end() {
	if (empty(t_args)) throw "args stack empty";
	return t_args.top().second;
}

size_t ArgsStack::size() const {
	if (empty(t_args)) throw "args stack empty";
	return std::distance(t_args.top().first, t_args.top().second);
}

void ArgsStack::clear() {
	while (!t_args.empty()) {
		t_args.pop();
	}
}