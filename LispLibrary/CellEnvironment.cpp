#include "CellEnvironment.h"
#include "Funcs.h"
using namespace std;

CellEnvironment::CellEnvironment(mp&& env):
	t_glonal(move(env))
{
}

CellEnvironment::CellEnvironment(const mp& env):
	t_glonal(env)
{
}

void CellEnvironment::push(frame&& rh)
{
	t_stack.push_back(move(rh));
	for (auto it = begin(t_stack.back()); it != end(t_stack.back()); ++it) {
		t_all_in_stack[it->first].push_back(&it->second);
	}
}

void CellEnvironment::push(const frame& rh)
{
	t_stack.push_back(rh);
	for (auto it = begin(t_stack.back()); it != end(t_stack.back()); ++it) {
		t_all_in_stack[it->first].push_back(&it->second);
	}
}

void CellEnvironment::pop()
{
	if (t_stack.empty()) {
		throw "CellEnvironment::pop: empty stack";
	}
	for (auto it = begin(t_stack.back()); it != end(t_stack.back()); ++it) {
		auto it2 = t_all_in_stack.find(it->first);
		it2->second.pop_back();
		if (it2->second.empty()) {
			t_all_in_stack.erase(it2);
		}
	}
	t_stack.pop_back();
}

size_t CellEnvironment::size() const
{
	return t_stack.size();
}

const CellEnvironment::mp& CellEnvironment::get_globals() const
{
	return t_glonal;
}

void CellEnvironment::add_global_var(const Symbol& name, const Cell& val)
{
	t_glonal.emplace(name, val);
}

Cell* CellEnvironment::get_global_var(const Symbol& name)
{
	if (auto it = t_glonal.find(name); it != end(t_glonal)) {
		return &it->second;
	}
	return nullptr;
}

const Cell* CellEnvironment::get_global_var(const Symbol& name) const
{
	if (auto it = t_glonal.find(name); it != end(t_glonal)) {
		return &it->second;
	}
	return nullptr;
}

Cell* CellEnvironment::get(const Symbol& name)
{
	if (!t_stack.empty()) {
		if (auto it = t_all_in_stack.find(name); it != end(t_all_in_stack)) {
			return it->second.back();
		}
	}
	return get_global_var(name);
}

const Cell* CellEnvironment::get(const Symbol& name) const
{
	if (!t_stack.empty()) {
		if (auto it = t_all_in_stack.find(name); it != end(t_all_in_stack)) {
			return it->second.back();
		}
	}
	return get_global_var(name);
}

void CellEnvironment::clear_subenvs()
{
	t_stack.clear();
	t_all_in_stack.clear();
}
