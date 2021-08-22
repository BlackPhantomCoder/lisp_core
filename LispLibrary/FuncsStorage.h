#pragma once
#include "LambdaCell.h"
#include "MacroCell.h"
#include "CoreData.h"

#include <variant>
#include <unordered_map>
#include <optional>

class FuncsStorage
{
public:
	struct nbifunc {
		CoreData::bifunc ptr = nullptr;
	};

	struct bifunc {
		CoreData::bifunc ptr = nullptr;
	};

	using data 
		= std::variant<lambda, macro, FuncsStorage::bifunc, FuncsStorage::nbifunc, CoreData::special_bifunc_make, CoreData::special_nbifunc_make>;

	using mp = std::unordered_map <
		Symbol,
		data
	>;
public:
	FuncsStorage(SExprsFarm& farm);

	void set_func(const Symbol& symbol, const lambda& cell);
	void set_func(const Symbol& symbol, const macro& cell);

	void set_func(const Symbol& symbol, lambda&& cell);
	void set_func(const Symbol& symbol, macro&& cell);

	std::optional<std::reference_wrapper<const FuncsStorage::data>> find(const Symbol & symbol);
private:
	SExprsFarm& t_farm;
	mp t_funcs;
};

