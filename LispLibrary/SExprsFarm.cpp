#include "SExprsFarm.h"
#include "SymbolsFarm.h"

#include "Symbol.h"
#include "Number.h"
#include "DotPair.h"

#include "Funcs.h"

#include "MakeSExprShare.h"
using namespace std;
using namespace CoreData;

SExprsFarm::SExprsFarm(CoreEnvironment& env):
	t_env(env)
{
	t_nil = make_symbol_cell(nil_str);
	t_T = make_symbol_cell(T_str);
	t_lambda_symbol = make_symbol_cell(lambda_str);
	t_nlambda_symbol = make_symbol_cell(nlambda_str);
	t_macro_symbol = make_symbol_cell(macro_str);
	t_read_up_case_symbol = make_symbol_cell(read_up_case_str);
}

Symbol SExprsFarm::make_symbol(const std::string& data)
{
	return Symbol(t_symbols.make_or_copy2(data));
}

Symbol SExprsFarm::make_symbol(std::string&& data)
{
	return Symbol(t_symbols.make_or_copy2(move(data)));
}

Number SExprsFarm::make_number(double val)
{
	return Number(val);
}

Number SExprsFarm::make_number(const BigInt& val)
{
	return Number(val);
}

Cell SExprsFarm::make_symbol_cell(const std::string& data)
{
	return Cell(make_SExprShare<Symbol>(t_symbols.make_or_copy2(data)));
}

Cell SExprsFarm::make_symbol_cell(std::string&& data)
{
	return Cell(make_SExprShare<Symbol>(t_symbols.make_or_copy2(move(data))));
}

Cell SExprsFarm::make_symbol_cell(Symbol&& s)
{
	return Cell(make_SExprShare<Symbol>(move(s)));
}

Cell SExprsFarm::make_symbol_cell(const Symbol& s)
{
	return Cell(make_SExprShare<Symbol>(s));
}

Cell SExprsFarm::make_number_cell(double val)
{
	return Cell(make_SExprShare<Number>(val));
}

Cell SExprsFarm::make_number_cell(const BigInt& val)
{
	return Cell(make_SExprShare<Number>(val));
}

Cell SExprsFarm::make_number_cell(Number&& s)
{
	return Cell(make_SExprShare<Number>(move(s)));
}

Cell SExprsFarm::make_number_cell(const Number& s)
{
	return Cell(make_SExprShare<Number>(s));
}

Cell SExprsFarm::make_empty_list_cell()
{
	return Cell(make_SExprShare<DotPair>());
}

Cell SExprsFarm::make_list_cell(const DotPair& dp)
{
	return Cell(make_SExprShare<DotPair>(dp));
}

Cell SExprsFarm::make_list_cell(const Cell& f)
{
	return make_list_cell(f, t_nil);
}

Cell SExprsFarm::make_list_cell(const Cell& f, const Cell& s)
{
	return Cell(make_SExprShare<DotPair>(f, s));
}

Cell SExprsFarm::make_list_cell(std::initializer_list<Cell> l)
{
	return make_list_cell(begin(l), end(l));
}

Cell SExprsFarm::read_up_case_symbol()
{
	return t_read_up_case_symbol;
}

Cell SExprsFarm::lambda_symbol()
{
	return t_lambda_symbol;
}

Cell SExprsFarm::nlambda_symbol()
{
	return t_nlambda_symbol;
}

Cell SExprsFarm::macro_symbol()
{
	return t_macro_symbol;
}

Cell SExprsFarm::nil()
{
	return t_nil;
}

Cell SExprsFarm::T()
{
	return t_T;
}

const SymbolsFarm::oblist& SExprsFarm::get_lst() const
{
	return t_symbols.get_lst();
}

void SExprsFarm::init(std::optional<std::reference_wrapper<nlohmann::json>> state)
{
	if (state) {
		load_state(*state);
	}
}

void SExprsFarm::save_state(nlohmann::json& j)
{
	t_symbols.save_state(j);
}

void SExprsFarm::load_state(const nlohmann::json& j)
{
	t_symbols.load_state(j);
}