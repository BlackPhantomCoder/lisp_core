#include "CellEnvironment.h"
#include "Funcs.h"
#include "CoreEnv.h"
using namespace std;

CellEnvironment::CellEnvironment(CoreEnvironment& env) :
	t_env(env)
{
}

void CellEnvironment::init(std::optional<std::reference_wrapper<nlohmann::json>> state)
{
	if (state) {
		load_state(*state);
	}
}

void CellEnvironment::push(const frame& rh)
{
	t_stack.push_back({});
	auto& [s, c] = rh;
	auto [it_s, end_s] = s;
	auto [it_c, end_c] = c;
	while (it_s != end_s && it_c != end_c) {
		if (!is_symbol(*it_s)) {
			++it_s;
			++it_c;
			continue;
		}
		t_stack.back().emplace_back(to_symbol(*it_s++), *it_c++);
		t_all_in_stack[t_stack.back().back().first].push_back({ t_stack.size() - 1, t_stack.back().size() - 1});
	}
	while (it_s != end_s) {
		if (!is_symbol(*it_s)) {
			++it_s;
			continue;
		}
		t_stack.back().push_back({ to_symbol(*it_s++), t_env.farm().nil() });
		t_all_in_stack[t_stack.back().back().first].push_back({ t_stack.size() - 1, t_stack.back().size() - 1});
	}
}

void CellEnvironment::pop()
{
	if (t_stack.empty()) {
		throw "CellEnvironment::pop: empty stack";
	}

	for (auto& [s, v] : t_stack.back()) {
		auto it = t_all_in_stack.find(s);
		it->second.pop_back();
		if (it->second.empty()) {
			t_all_in_stack.erase(it);
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
			const auto& [i, j] = it->second.back();
			return t_stack[i][j].second;
		}
	}
	return get_global_var(name);
}

void CellEnvironment::set(const Symbol& name, const Cell& val)
{
	if (auto it = t_all_in_stack.find(name); it != end(t_all_in_stack)) {
		const auto& [i, j] = it->second.back();
		t_stack[i][j].second = val;
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

void CellEnvironment::save_state(nlohmann::json& j)
{
	auto& jglobals = j.emplace_back();

	for (const auto& [symbol, val] : t_glonal) {
		jglobals.emplace_back(symbol.to_string());
		jglobals.emplace_back(val);
	}

}

void CellEnvironment::load_state(const nlohmann::json& j)
{
	t_glonal.clear();
	const auto& jglobals = j.at(0);

	for (auto it = begin(jglobals); it != end(jglobals); it += 2) {
		t_glonal[t_env.farm().make_symbol((*it).get<string>())] = t_env.cserial().in(*next(it));
	}
}
