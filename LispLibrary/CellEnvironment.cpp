#include "CellEnvironment.h"

using namespace std;

CellEnvironment::CellEnvironment(std::unordered_map<std::string, Cell>&& env):
    t_env(move(env))
{
}

CellEnvironment::CellEnvironment(const std::unordered_map<std::string, Cell>& env) :
    t_env(env)
{
}

CellEnvironment::CellEnvironment(std::unordered_map<std::string, Cell>&& env, CellEnvironment& prev):
    t_env(move(env)),
    t_prev(make_optional<reference_wrapper<CellEnvironment>>(reference_wrapper<CellEnvironment>{prev})),
    t_prev_const(make_optional<reference_wrapper<const CellEnvironment>>(reference_wrapper<const CellEnvironment>{ prev }))
{
}


CellEnvironment::CellEnvironment(const std::unordered_map<std::string, Cell>& env, CellEnvironment& prev) :
    t_env(env),
    t_prev(make_optional<reference_wrapper<CellEnvironment>>(reference_wrapper<CellEnvironment>{prev})),
    t_prev_const(make_optional<reference_wrapper<const CellEnvironment>>(reference_wrapper<const CellEnvironment>{ prev }))
{
}

std::optional<std::reference_wrapper<Cell>> CellEnvironment::get(const std::string& name)
{
    if (auto it = t_env.find(name); it != end(t_env)) {
        return make_optional<reference_wrapper<Cell>>(it->second);
    }
    if (t_prev) return t_prev->get().get(name);
    return nullopt;
}

std::optional<std::reference_wrapper<const Cell>> CellEnvironment::get(const std::string& name) const
{
    if (auto it = t_env.find(name); it != end(t_env)) {
        return make_optional<reference_wrapper<const Cell>>(it->second);
    }
    if (t_prev) return t_prev_const->get().get(name);
    return nullopt;
}
