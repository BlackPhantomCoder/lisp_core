#pragma once
#include <string>
#include <unordered_map>
#include <optional>

#include "Cell.h"

//environment for nlambda/lambda 
class CellEnvironment
{
public:
	explicit CellEnvironment(std::unordered_map<std::string, Cell>&& env);
	explicit CellEnvironment(const std::unordered_map<std::string, Cell>& env);

	explicit CellEnvironment(std::unordered_map<std::string, Cell>&& env, CellEnvironment& prev);
	explicit CellEnvironment(const std::unordered_map<std::string, Cell>& env, CellEnvironment& prev);

	std::optional<std::reference_wrapper<Cell>> get(const std::string& name);
	std::optional<std::reference_wrapper<const Cell>> get(const std::string& name) const;
	
private:
	std::unordered_map<std::string, Cell> t_env;
	std::optional<std::reference_wrapper<CellEnvironment>> t_prev;
	std::optional<std::reference_wrapper<const CellEnvironment>> t_prev_const;
};

