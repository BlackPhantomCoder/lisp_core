#pragma once
#include "Cell.h"
#include "CoreData.h"
#include <vector>
#include <unordered_map>

class SymbolsProvider {
public:
	Symbols symbols;

	Symbol nil_symbol;
	Symbol T_symbol;
	Symbol read_up_case_symbol;

	Symbol lambda_symbol;
	Symbol nlambda_symbol;

	const Cell nil_list;
	const Cell nil;
	const Cell T;

	static const CoreData::bifuncs_array bifuncs_arr;
	static const CoreData::nbifuncs_array nbifuncs_arr;
	std::unordered_map<Symbol, CoreData::bifunc> bifuncs;
	std::unordered_map<Symbol, CoreData::bifunc> nbifuncs;
public:
	SymbolsProvider();

private:
	//std::vector<Symbol> t_some_symbols;
};

