#pragma once
#include "LambdaCell.h"
#include "CoreData.h"

#include <variant>
#include <unordered_map>


class FuncsStorage
{
public:
	struct nbifunc {
		CoreData::bifunc ptr = nullptr;
	};

	struct bifunc {
		CoreData::bifunc ptr = nullptr;
	};

	using mp = std::unordered_map <
		Symbol,
		std::variant<lambda, FuncsStorage::bifunc, FuncsStorage::nbifunc, CoreData::special_bifunc_make, CoreData::special_nbifunc_make>
	>;
public:
	FuncsStorage(SExprsFarm& farm);

	void add_lambda(const Symbol& symbol, const lambda& cell);
	void add_lambda(const Symbol & symbol, lambda && cell);

	std::variant<
		std::reference_wrapper<lambda>,
		FuncsStorage::bifunc,
		FuncsStorage::nbifunc,
		CoreData::special_bifunc_make,
		CoreData::special_nbifunc_make,
		std::monostate
	> find(const Symbol & symbol);
private:
	SExprsFarm& t_farm;
	mp t_funcs;
};

