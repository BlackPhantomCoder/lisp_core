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
private:
	using mp = std::unordered_map <Symbol, std::variant<lambda, FuncsStorage::bifunc, FuncsStorage::nbifunc>>;
	friend void make_bifuncs(mp& mp, SExprsFarm& farm);
	friend void  make_nbifuncs(mp& mp, SExprsFarm& farm);

	static const CoreData::bifuncs_array bifuncs_arr;
	static const CoreData::nbifuncs_array nbifuncs_arr;
public:
	FuncsStorage(SExprsFarm& farm);

	void add_lambda(const Symbol& symbol, const lambda& cell);
	void add_lambda(const Symbol & symbol, lambda && cell);

	std::variant<std::reference_wrapper<lambda>, FuncsStorage::bifunc, FuncsStorage::nbifunc, std::monostate> find(const Symbol & symbol);
private:
	SExprsFarm& t_farm;
	std::unordered_map <Symbol, std::variant<lambda, FuncsStorage::bifunc, FuncsStorage::nbifunc>> t_funcs;
};

