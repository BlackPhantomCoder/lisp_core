#include "SExprsFarm.h"
#include "SymbolsFarm.h"

#include "Symbol.h"
#include "Number.h"
#include "DotPair.h"

#include "Funcs.h"

using namespace std;
using namespace CoreData;

SExprsFarm::SExprsFarm(CoreEnvironment* env):
	t_env(env),
	symbols(std::make_unique<SymbolsFarm>())
{
	read_up_case_symbol = make_symbol_cell(read_up_case_str);
	lambda_symbol = make_symbol_cell(lambda_str);
	nlambda_symbol = make_symbol_cell(nlambda_str);
	nil = make_symbol_cell(nil_str);
	T =make_symbol_cell(T_str);
}

Symbol SExprsFarm::make_symbol(const std::string& data)
{
	Symbol c;
	c.t_data = symbols->make_or_copy(data);
	return c;
}

Symbol SExprsFarm::make_symbol(std::string&& data)
{
	Symbol c;
	c.t_data = symbols->make_or_copy(move(data));
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
	auto c = make_cell_list_noinit();
	t_init_empty_list(to_list(c));
	return c;
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

void SExprsFarm::t_init_list(DotPair& p, const Cell& f, const Cell& s)
{
	p.t_symbols = this;
	p.t_first = f;
	p.t_second = s;
}

void SExprsFarm::t_init_symb(Symbol& s, const std::string& data)
{
	s.t_data = symbols->make_or_copy(data);
}

void SExprsFarm::t_init_symb(Symbol& s, std::string&& data)
{
	s.t_data = symbols->make_or_copy(move(data));
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
	return Cell(make_SExprCore_list_noinit());
}

Cell SExprsFarm::make_cell_symb_noinit()
{
	return Cell(make_SExprCore_symb_noinit());
}

Cell SExprsFarm::make_cell_numb_noinit()
{
	return Cell(make_SExprCore_numb_noinit());
}

void SExprsFarm::t_init_empty_list(DotPair& d)
{
	d.t_symbols = this;
}

