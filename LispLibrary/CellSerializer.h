#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <variant>

#include "Cell.h"
#include "LambdaCell.h"

#include "json/include/json.hpp"

class Number;
class Symbol;
class DotPair;
class CoreEnvironment;

// сохранение объектов в json
class CellSerializer
{
public:
	CellSerializer(CoreEnvironment& env);

	void out(nlohmann::json& j, const lambda& l);
	void out(nlohmann::json& j, const Cell& cell);

	lambda in_lambda(const nlohmann::json& j);
	Cell in(const nlohmann::json& j);

private:
	CoreEnvironment& t_env;
};


void to_json(nlohmann::json& j, const Cell& cell);