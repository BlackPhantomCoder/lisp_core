#include "CellEnvironment.h"
#include "Funcs.h"
using namespace std;

CellEnvironment::CellEnvironment(SExprsFarm& farm):
	t_farm(farm)
{
}

void CellEnvironment::push(frame&& rh)
{
	t_stack.push_back(move(rh));
	auto& [s, c] = t_stack.back();
	auto [it_s, end_s] = s;
	auto [it_c, end_c] = c;
	while (it_s != end_s && it_c != end_c) {
		if (!is_symbol(*it_s)) {
			++it_s;
			continue;
		}
		t_all_in_stack[to_symbol(*it_s++)].push_back(&*it_c++);
	}
	while (it_s != end_s) {
		if (!is_symbol(*it_s)) {
			++it_s;
			continue;
		}
		t_all_in_stack[to_symbol(*it_s++)].push_back(&t_farm.nil);
	}
}

void CellEnvironment::push(const frame& rh)
{
	t_stack.push_back(rh);
	auto& [s, c] = t_stack.back();
	auto [it_s, end_s] = s;
	auto [it_c, end_c] = c;
	while(it_s != end_s && it_c != end_c){
		if (!is_symbol(*it_s)) {
			++it_s;
			continue;
		}
		t_all_in_stack[to_symbol(*it_s++)].push_back(&*it_c++);
	}
	while (it_s != end_s) {
		if (!is_symbol(*it_s)) {
			++it_s;
			continue;
		}
		t_all_in_stack[to_symbol(*it_s++)].push_back(&t_farm.nil);
	}
}

void CellEnvironment::pop()
{
	if (t_stack.empty()) {
		throw "CellEnvironment::pop: empty stack";
	}

	auto& [s, c] = t_stack.back();
	auto [it_s, end_s] = s;

	for (; it_s != end_s; ++it_s) {
		if (!is_symbol(*it_s)) continue;
		auto it2 = t_all_in_stack.find(to_symbol(*it_s));
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
	t_all_in_stack = {};
	while (!empty(t_stack)) {
		t_stack.back() = {};
		t_stack.pop_back();
	}
	t_stack.shrink_to_fit();
}
