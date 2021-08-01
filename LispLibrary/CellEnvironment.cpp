#include "CellEnvironment.h"

using namespace std;

CellEnvironment::CellEnvironment(mp&& env):
	t_glonal(move(env))
{
}

CellEnvironment::CellEnvironment(const mp& env):
	t_glonal(env)
{
}

void CellEnvironment::push(mp&& rh)
{
	t_stack.push_back(move(rh));
}

void CellEnvironment::push(const mp& rh)
{
	t_stack.push_back(rh);
}

void CellEnvironment::pop()
{
	if (t_stack.empty()) {
		throw "CellEnvironment::pop: empty stack";
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

std::optional<std::reference_wrapper<Cell>> CellEnvironment::get_global_var(const Symbol& name)
{
	if (auto it = t_glonal.find(name); it != end(t_glonal)) {
		return make_optional<std::reference_wrapper<Cell>>(std::reference_wrapper<Cell>{ it->second });
	}
	return nullopt;
}

std::optional<std::reference_wrapper<const Cell>> CellEnvironment::get_global_var(const Symbol& name) const
{
	if (auto it = t_glonal.find(name); it != end(t_glonal)) {
		return make_optional<std::reference_wrapper<const Cell>>(std::reference_wrapper<const Cell>{ it->second });
	}
	return nullopt;
}

std::optional<std::reference_wrapper<Cell>> CellEnvironment::get(const Symbol& name)
{
	if (!t_stack.empty()) {
		for (auto s_it = prev(end(t_stack)); s_it != begin(t_stack); --s_it) {
			if (auto it = s_it->find(name); it != end(*s_it)) {
				return make_optional<std::reference_wrapper<Cell>>(std::reference_wrapper< Cell>{ it->second });
			}
		}
		if (auto it = begin(t_stack)->find(name); it != end(*begin(t_stack))) {
			return make_optional<std::reference_wrapper<Cell>>(std::reference_wrapper< Cell>{ it->second });
		}
	}
	return get_global_var(name);
}

std::optional<std::reference_wrapper<const Cell>> CellEnvironment::get(const Symbol& name) const
{
	if (!t_stack.empty()) {
		for (auto s_it = prev(end(t_stack)); s_it != begin(t_stack); --s_it) {
			if (auto it = s_it->find(name); it != end(*s_it)) {
				return make_optional<std::reference_wrapper<const Cell>>(std::reference_wrapper<const Cell>{ it->second });
			}
		}
		if (auto it = begin(t_stack)->find(name); it != end(*begin(t_stack))) {
			return make_optional<std::reference_wrapper<const Cell>>(std::reference_wrapper<const Cell>{ it->second });
		}
	}
	return get_global_var(name);
}

void CellEnvironment::clear_subenvs()
{
	t_stack.clear();
}
