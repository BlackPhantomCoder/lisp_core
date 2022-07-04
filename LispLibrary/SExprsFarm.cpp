#include "SExprsFarm.h"
#include "SymbolsFarm.h"

#include "Symbol.h"
#include "Number.h"
#include "DotPair.h"

#include "Funcs.h"

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
	Symbol c;
	t_init_symb(c, data);
	return c;
}

Symbol SExprsFarm::make_symbol(std::string&& data)
{
	Symbol c;
	t_init_symb(c, move(data));
	return c;
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
	auto c = make_cell_symb_noinit();
	t_init_symb(to_symbol(c), data);
	return c;
}

Cell SExprsFarm::make_symbol_cell(std::string&& data)
{
	auto c = make_cell_symb_noinit();
	t_init_symb(to_symbol(c), move(data));
	return c;
}

Cell SExprsFarm::make_symbol_cell(Symbol&& s)
{
	auto c = make_cell_symb_noinit();
	to_symbol(c) = move(s);
	return c;
}

Cell SExprsFarm::make_symbol_cell(const Symbol& s)
{
	auto c = make_cell_symb_noinit();
	to_symbol(c) = s;
	return c;
}

Cell SExprsFarm::make_number_cell(double val)
{
	auto c = make_cell_numb_noinit();
	t_init_numb(to_number(c), val);
	return c;
}

Cell SExprsFarm::make_number_cell(const BigInt& val)
{
	auto c = make_cell_numb_noinit();
	t_init_numb(to_number(c), val);
	return c;
}

Cell SExprsFarm::make_number_cell(Number&& s)
{
	auto c = make_cell_numb_noinit();
	to_number(c) = move(s);
	return c;
}

Cell SExprsFarm::make_number_cell(const Number& s)
{
	auto c = make_cell_numb_noinit();
	to_number(c) = s;
	return c;
}

Cell SExprsFarm::make_empty_list_cell()
{
	// пустой лист не нуждается в инициализации
	return make_cell_list_noinit();
}

Cell SExprsFarm::make_list_cell(const DotPair& dp)
{
	auto c = make_cell_list_noinit();
	to_list(c) = dp;
	return c;
}

Cell SExprsFarm::make_list_cell(const Cell& f)
{
	return make_list_cell(f, t_nil);
}

Cell SExprsFarm::make_list_cell(const Cell& f, const Cell& s)
{
	auto c = make_cell_list_noinit();
	t_init_list(to_list(c), f, s);
	return c;
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

void SExprsFarm::t_init_list(DotPair& p, const Cell& f, const Cell& s)
{
	p.t_first = f;
	p.t_second = s;
}

int i = 0;

void SExprsFarm::t_init_symb(Symbol& s, const std::string& data)
{
	t_symbols.make_or_copy(s, data);
}

void SExprsFarm::t_init_symb(Symbol& s, std::string&& data)
{
	t_symbols.make_or_copy(s, move(data));
}

void SExprsFarm::t_init_numb(Number& n, double val)
{
	n.t_data = { val };
}

void SExprsFarm::t_init_numb(Number& n, const BigInt& val)
{
	n.t_data = { val };
}


Cell SExprsFarm::make_cell_list_noinit()
{
	return Cell(make_SExprShare_list_noinit());
}

Cell SExprsFarm::make_cell_symb_noinit()
{
	return Cell(make_SExprShare_symb_noinit());
}

Cell SExprsFarm::make_cell_numb_noinit()
{
	return Cell(make_SExprShare_numb_noinit());
}
