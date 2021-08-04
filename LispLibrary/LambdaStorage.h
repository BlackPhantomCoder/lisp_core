#pragma once
#include "LambdaCell.h"

#include <unordered_map>
#include <optional>

class LambdaStorage
{
public:
	LambdaStorage() = default;
	void add(const Symbol& symbol, const lambda& cell);
	void add(const Symbol& symbol, lambda&& cell);

	std::optional<std::reference_wrapper<const lambda>> find(const Symbol& symbol)const;
private:
	std::unordered_map <Symbol, lambda> t_lambdas;
};

