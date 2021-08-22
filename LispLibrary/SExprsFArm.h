#pragma once
//#include "SymbolsFerm.h"
//#include "SymbolsProvider.h"
#include <memory>
#include <string>
#include "Cell.h"

class CoreEnvironment;
class BigInt;
class SymbolsFarm;

class SExprsFarm
{
public:
	std::unique_ptr<SymbolsFarm> symbols;
	
public:
	SExprsFarm(CoreEnvironment& env);

	Symbol make_symbol(const std::string& data);
	Symbol make_symbol(std::string&& data);

	Number make_number(double val);
	Number make_number(const BigInt& val);

	//Cells

	//Symbol
	Cell make_symbol_cell(const std::string& data);
	Cell make_symbol_cell(std::string&& data);

	Cell make_symbol_cell(Symbol&& s);
	Cell make_symbol_cell(const Symbol& s);

	//Number
	Cell make_number_cell(double val);
	Cell make_number_cell(const BigInt& val);

	Cell make_number_cell(Number&& s);
	Cell make_number_cell(const Number& s);

	//DotPair
	Cell make_empty_list_cell();

	//list with 1 element
	Cell make_list_cell(const Cell& f);
	Cell make_list_cell(const Cell& f, const Cell& s);

	Cell make_list_cell(std::initializer_list<Cell> l);

	template< class InputIt >
	Cell make_list_cell(InputIt first, InputIt last);

	Cell read_up_case_symbol();
	Cell lambda_symbol();
	Cell nlambda_symbol();
	Cell macro_symbol();
	Cell nil();
	Cell T();
private:
	void t_init_list(DotPair& p, const Cell& f, const Cell& s);
	void t_init_empty_list(DotPair& d);

	void t_init_symb(Symbol& s, const std::string& data);
	void t_init_symb(Symbol& s, std::string&& data);

	void t_init_numb(Number& n, double val);
	void t_init_numb(Number& n, const BigInt& val);

	Cell make_cell_list_noinit();
	Cell make_cell_symb_noinit();
	Cell make_cell_numb_noinit();
private:
	CoreEnvironment& t_env;
	Cell t_read_up_case_symbol;
	Cell t_lambda_symbol;
	Cell t_nlambda_symbol;
	Cell t_macro_symbol;
	Cell t_nil;
	Cell t_T;
};