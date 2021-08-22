#include "CellEnvironment.h"
#include "Funcs.h"
using namespace std;

CellEnvironment::CellEnvironment(SExprsFarm& farm):
	t_farm(farm)
{
}

void CellEnvironment::push(frame&& rh)
{
	t_stack.push_back({ move(rh), {} });
	auto& [s, c] = t_stack.back().first;
	auto [it_s, end_s] = s;
	auto [it_c, end_c] = c;
	while (it_s != end_s && it_c != end_c) {
		if (!is_symbol(*it_s)) {
			++it_s;
			++it_c;
			continue;
		}
		t_all_in_stack[to_symbol(*it_s++)].push_back(&*it_c++);
	}
	while (it_s != end_s) {
		if (!is_symbol(*it_s)) {
			++it_s;
			continue;
		}
		t_stack.back().second.push_back(t_farm.nil());
		t_all_in_stack[to_symbol(*it_s++)].push_back(&t_stack.back().second.back());
	}
}

void CellEnvironment::push(const frame& rh)
{
	t_stack.push_back({ move(rh), {} });
	auto& [s, c] = t_stack.back().first;
	auto [it_s, end_s] = s;
	auto [it_c, end_c] = c;
	while (it_s != end_s && it_c != end_c) {
		if (!is_symbol(*it_s)) {
			++it_s;
			++it_c;
			continue;
		}
		t_all_in_stack[to_symbol(*it_s++)].push_back(&*it_c++);
	}
	while (it_s != end_s) {
		if (!is_symbol(*it_s)) {
			++it_s;
			continue;
		}
		t_stack.back().second.push_back(t_farm.nil());
		t_all_in_stack[to_symbol(*it_s++)].push_back(&t_stack.back().second.back());
	}
}

void CellEnvironment::pop()
{
	if (t_stack.empty()) {
		throw "CellEnvironment::pop: empty stack";
	}

	auto& [s, c] = t_stack.back().first;
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

//void CellEnvironment::push(const frame& rh)
//{
//	t_stack.push_back({});
//	auto [s, c] = rh;
//	auto [it_s, end_s] = s;
//	auto [it_c, end_c] = c;
//	while(it_s != end_s && it_c != end_c){
//		if (!is_symbol(*it_s)) {
//			++it_s;
//			++it_c;
//			continue;
//		}
//		auto it = t_stack.back().emplace(to_symbol(*it_s++), *it_c++);
//		t_all_in_stack[it.first->first].push_back(&it.first->second);
//	}
//	while (it_s != end_s) {
//		if (!is_symbol(*it_s)) {
//			++it_s;
//			++it_c;
//			continue;
//		}
//		auto it = t_stack.back().emplace(to_symbol(*it_s++), t_farm.nil());
//		t_all_in_stack[it.first->first].push_back(&it.first->second);
//	}
//}

//void CellEnvironment::pop()
//{
//	if (t_stack.empty()) {
//		throw "CellEnvironment::pop: empty stack";
//	}
//
//	for (const auto& [s, val] : t_stack.back()) {
//		auto it2 = t_all_in_stack.find(s);
//		it2->second.pop_back();
//		if (it2->second.empty()) {
//			t_all_in_stack.erase(it2);
//		}
//	}
//	t_stack.pop_back();
//}

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

Cell CellEnvironment::get_global_var(const Symbol& name)
{
	if (auto it = t_glonal.find(name); it != end(t_glonal)) {
		return it->second;
	}
	return Cell();
}

Cell CellEnvironment::get(const Symbol& name)
{
	if (!t_stack.empty()) {
		if (auto it = t_all_in_stack.find(name); it != end(t_all_in_stack)) {
			return *it->second.back();
		}
	}
	return get_global_var(name);
}

void CellEnvironment::set(const Symbol& name, const Cell& val)
{
	if (auto it = t_all_in_stack.find(name); it != end(t_all_in_stack)) {

		*it->second.back() = val;
	}
	else {
		t_glonal[name] = val;
	}
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
