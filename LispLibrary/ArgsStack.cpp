#include "ArgsStack.h"

void ArgsStack::push(DPair::const_iterator begin, DPair::const_iterator end)
{
	t_args.push({ begin, end });
}

void ArgsStack::pop() {
	if (t_args.empty()) throw "args stack empty";
	t_args.pop();
}

DPair::const_iterator ArgsStack::begin() {
	if (t_args.empty()) throw "args stack empty";
	return t_args.top().first;
}

DPair::const_iterator ArgsStack::end() {
	if (t_args.empty()) throw "args stack empty";
	return t_args.top().second;
}

size_t ArgsStack::size() const {
	if (t_args.empty()) throw "args stack empty";
	return std::distance(t_args.top().first, t_args.top().second);
}

void ArgsStack::clear() {
	while (!t_args.empty()) {
		t_args.pop();
	}
}