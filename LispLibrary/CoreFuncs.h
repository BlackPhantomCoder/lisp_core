#pragma once
#include "Cell.h"
#include "Number.h"
#include "Symbol.h"
#include "DotPair.h"
#include "LambdaCell.h"
#include "MacroCell.h"

class CoreEnvironment;
class CoreFuncs
{
public:
	CoreFuncs(CoreEnvironment& env);


	lambda make_spread_lambda_form(
		lambda_types lambda_type,
		const Cell& params,
		Cell& body
	);

	lambda make_nospread_lambda_form(
		lambda_types lambda_type,
		const Symbol& param,
		Cell& body
	);

	macro make_spread_macro_form(
		const Cell& params,
		const Cell& body
	);

	macro make_nospread_macro_form(
		const Symbol& param,
		const Cell& body
	);


	Cell eval_atom(const Cell& atom);
	Cell eval_symbol(const Cell& symbol);
	void set_value(const Cell& name, const Cell& val);
	lambda make_lambda_form(Cell& lst);
private:
	CoreEnvironment& t_env;
};