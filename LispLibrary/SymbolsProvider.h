#pragma once
#include "Cell.h"
#include "CoreData.h"
#include <vector>

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

	static const CoreData::bifuncs_array bifuncs;
	static const CoreData::nbifuncs_array nbifuncs;

public:
	SymbolsProvider();

private:
	std::vector<Symbol> t_some_symbols;
};

