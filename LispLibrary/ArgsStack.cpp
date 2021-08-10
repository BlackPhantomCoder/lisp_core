#include "ArgsStack.h"

void ArgsStack::push(ArgsStack::iterator begin, ArgsStack::iterator end)
{
	t_args.push({ begin, end });
}

void ArgsStack::pop() {
	if (empty(t_args)) throw "args stack empty";
	t_args.pop();
}

ArgsStack::iterator ArgsStack::begin() {
	if (empty(t_args)) throw "args stack empty";
	return t_args.top().first;
}

ArgsStack::iterator ArgsStack::end() {
	if (empty(t_args)) throw "args stack empty";
	return t_args.top().second;
}

long presize(CarCdrIterator beg, CarCdrIterator end, long val) {
	long size = 0;
	while (size <= val && beg !=end) {
		++beg;
		++size;
	}
	return size;
}

bool ArgsStack::operator==(long val) const
{
	if (empty(t_args)) throw "args stack empty";
	return presize(t_args.top().first, t_args.top().second, val) == val;
}

bool ArgsStack::operator>=(long val) const
{
	if (empty(t_args)) throw "args stack empty";
	return  presize(t_args.top().first, t_args.top().second, val) >= val;
}

bool ArgsStack::operator<=(long val) const
{
	if (empty(t_args)) throw "args stack empty";
	return  presize(t_args.top().first, t_args.top().second, val) <= val;
}

bool ArgsStack::operator>(long val) const
{
	if (empty(t_args)) throw "args stack empty";
	return presize(t_args.top().first, t_args.top().second, val) > val;
}

bool ArgsStack::operator<(long val) const
{
	return  presize(t_args.top().first, t_args.top().second, val) < val;
}

//size_t ArgsStack::size() const {
//	if (empty(t_args)) throw "args stack empty";
//	return std::distance(t_args.top().first, t_args.top().second);
//}

void ArgsStack::clear() {
	while (!t_args.empty()) {
		t_args.pop();
	}
}