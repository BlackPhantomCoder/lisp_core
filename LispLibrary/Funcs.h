#pragma once
#include "Cell.h"
#include "DotPair.h"
#include <list>
#include <string>
#include <iostream>
#include "PublicCoreEnvironmentProvider.h"
#include "SymbolsProvider.h"
#include "CarCdrIterator.h"
std::ostream& operator<<(std::ostream& s, const Cell& exp);

std::string to_string(const Cell& exp);
std::string to_string(const DotPair& exp);
std::string to_string(const Atom& exp);
std::string to_string(const Symbol& exp);
std::string to_string(const Number& exp);

inline Cell bool_cast(bool val, SymbolsProvider& provider);
inline bool is_null(const Cell& c, SymbolsProvider& provider);
inline bool is_null_symbol(const Cell& c, SymbolsProvider& provider);
inline bool is_null_symbol(const Atom& c, SymbolsProvider& provider);
inline bool is_null_symbol(const Symbol& c, SymbolsProvider& provider);
inline bool is_T(const Cell& c, SymbolsProvider& provider);
inline bool is_implicit_cond(const Cell& arg, SymbolsProvider& provider);

inline bool is_alambda_form(const Cell& arg, SymbolsProvider& provider);
inline bool is_lambda_form(const Cell& arg, SymbolsProvider& provider);
inline bool is_nlambda_form(const Cell& arg, SymbolsProvider& provider);

inline bool is_alambda_form(const DotPair& arg, SymbolsProvider& provider);
inline bool is_lambda_form(const DotPair& arg, SymbolsProvider& provider);
inline bool is_nlambda_form(const DotPair& arg, SymbolsProvider& provider);

inline bool is_alambda_symbol(const Symbol& arg, SymbolsProvider& provider);










inline DotPair& to_list(Cell& c) {
    return c.to_list();
}

inline const DotPair& to_list(const Cell& c) {
    return c.to_list();
}



inline bool is_null(const Cell& c, SymbolsProvider& provider) {
    return (is_null_symbol(c, provider) || (is_list(c) && std::empty(to_list(c))));
}

inline bool is_null_symbol(const Cell& c, SymbolsProvider& provider) {
    return is_symbol(c) && is_null_symbol(to_symbol(c), provider);
}

inline bool is_null_symbol(const Atom& c, SymbolsProvider& provider){
    return is_symbol(c) && is_null_symbol(to_symbol(c), provider);
}

inline bool is_null_symbol(const Symbol& c, SymbolsProvider& provider) {
    return c == provider.nil_symbol;
}

inline Cell bool_cast(bool val, SymbolsProvider& provider) {
    return (val) ? provider.T : provider.nil;
}

inline bool is_T(const Cell& c, SymbolsProvider& provider) {
    return is_symbol(c) && to_symbol(c) == provider.T_symbol;
}

inline bool is_implicit_cond(const Cell& arg, SymbolsProvider& provider)
{
    if (!is_list(arg)) return false;
    if (is_null(arg, provider)) return false;
    if (!is_list(car(to_list(arg)))) return false;
    if (is_null(car(to_list(arg)), provider)) return true;
    if (!is_symbol(car(car(to_list(arg))))) return true;
    if (is_alambda_symbol(to_symbol(car(car(to_list(arg)))), provider)) return false;
    return true;
}

inline bool is_alambda_form(const Cell& arg, SymbolsProvider& provider)
{
    return is_list(arg) && !is_null_symbol(arg, provider) && is_alambda_form(to_list(arg), provider);
}

inline bool is_lambda_form(const Cell& arg, SymbolsProvider& provider)
{
    return is_list(arg) && !is_null_symbol(arg, provider) && is_nlambda_form(to_list(arg), provider);
}

inline bool is_nlambda_form(const Cell& arg, SymbolsProvider& provider)
{
    return is_list(arg) && !is_null_symbol(arg, provider) && is_nlambda_form(to_list(arg), provider);
}

inline bool is_alambda_form(const DotPair& arg, SymbolsProvider& provider)
{
    return !std::empty(arg) && is_symbol(car(arg)) && is_alambda_symbol(to_symbol(car(arg)), provider);       
}

inline bool is_lambda_form(const DotPair& arg, SymbolsProvider& provider)
{
    return !std::empty(arg) && is_symbol(car(arg)) && to_symbol(car(arg)) == provider.lambda_symbol;
}

inline bool is_nlambda_form(const DotPair& arg, SymbolsProvider& provider)
{
    return !std::empty(arg) && is_symbol(car(arg)) && to_symbol(car(arg)) == provider.nlambda_symbol;
}

inline bool is_alambda_symbol(const Symbol& arg, SymbolsProvider& provider)
{
    return arg == provider.lambda_symbol || arg == provider.nlambda_symbol;
}




inline Cell deep_copy_list_cell(const DotPair& lst)
{
    return Cell(deep_copy_list(lst));
}

inline Cell make_list_cell(std::initializer_list<Cell> l, SymbolsProvider& provider)
{
    return Cell(make_list(l, provider));
}

inline Cell make_list_cell(DotPair&& objs)
{
    return Cell(std::move(objs));
}

inline Cell make_list_cell(const DotPair& objs)
{
    return Cell(objs);
}

inline Cell make_number(Number&& numb)
{
    return Cell(Atom(std::move(numb)));
}

inline Cell make_number(const Number& numb)
{
    return Cell(Atom(numb));
}

inline Cell make_symbol_cell(Atom&& symbol) {
    return Cell(std::move(symbol));
}

inline Cell make_symbol_cell(const Atom& symbol) {
    return Cell(symbol);
}

inline Cell make_symbol_cell(Symbol&& symbol)
{
    return Cell(Atom(std::move(symbol)));
}

inline Cell make_symbol_cell(const Symbol& symbol)
{
    return Cell(Atom(Symbol(symbol)));
}

inline Cell make_symbol_cell(std::string&& symbol, Symbols& owner)
{
    return Cell(Atom(make_symbol(std::move(symbol), owner)));
}

inline Cell make_symbol_cell(const std::string& symbol, Symbols& owner)
{
    return Cell(Atom(make_symbol(symbol, owner)));
}

inline bool is_real_number(const Cell& c)
{
    return is_number(c) && (to_number(c)).is_real();
}

inline bool is_int_number(const Cell& c)
{
    return is_number(c) && (to_number(c)).is_integer();
}

inline bool is_number(const Cell& c)
{
    return is_atom(c) && to_atom(c).is_number();
}

inline bool is_number(const Atom& c)
{
    return c.is_number();
}

inline bool is_atom(const Cell& c)
{
    return c.is_atom();
}

inline bool is_symbol(const Cell& c)
{
    return is_atom(c) && to_atom(c).is_symbol();
}

inline bool is_symbol(const Atom& c)
{
    return c.is_symbol();
}

inline bool is_list(const Cell& c)
{
    return c.is_list();
}

inline Cell make_atom(Atom&& atom) {
    return Cell(std::move(atom));
}

inline Cell make_atom(const Atom& atom) {
    return Cell(atom);
}

template<class InputIt>
inline Cell make_list_cell(InputIt first, InputIt last, SymbolsProvider& provider)
{
    return make_list<InputIt>(first, last, provider);
}



template<class InputIt>
inline DotPair make_list(InputIt first, InputIt last, SymbolsProvider& provider)
{
    if (first == last) return make_empty_list(provider);
    --last;
    DotPair result = make_empty_list(provider);
    for (; first != last; --last)
    {
        result = cons(*last, make_list_cell(result), provider);
    }
    result = cons(*last, make_list_cell(result), provider);
    return result;
}

inline Cell& car(DotPair& dp)
{
    if (std::empty(dp)) throw "";
    return dp.t_core->t_f;
}

inline Cell& cdr(DotPair& dp)
{
    if (std::empty(dp)) throw "";
    return dp.t_core->t_s;
}

inline Cell& car(Cell& dp)
{
    if (std::empty(to_list(dp))) return dp;
    return to_list(dp).t_core->t_f;
}

inline Cell& cdr(Cell& dp)
{
    if (std::empty(to_list(dp))) return dp;
    return to_list(dp).t_core->t_s;
}

inline const Cell& car(const DotPair& dp)
{
    if (std::empty(dp)) return dp.t_symbols.get().nil;
    return dp.t_core->t_f;
}

inline const Cell& cdr(const DotPair& dp)
{
    if (std::empty(dp)) return dp.t_symbols.get().nil;
    return dp.t_core->t_s;
}

inline const Cell& car(const Cell& dp)
{
    if (std::empty(to_list(dp))) return to_list(dp).t_symbols.get().nil;
    return to_list(dp).t_core->t_f;
}

inline const Cell& cdr(const Cell& dp)
{
    if (std::empty(to_list(dp))) return to_list(dp).t_symbols.get().nil;
    return to_list(dp).t_core->t_s;
}

inline DotPair cons(const Cell& f, const Cell& s, SymbolsProvider& provider)
{
    return DotPair(DotPairCore(f, s), provider);
}

inline Cell cons_cell(const Cell& f, const Cell& s, SymbolsProvider& provider)
{
    return Cell(cons(f, s, provider));
}

//inline DotPair append(const Cell& f, const Cell& s)
//{
//	if (is_atom(f)) return s;
//	return cons(car(f), append_cell(cdr(f), s), provider);
//}

inline Cell append_cell(const Cell& f, const Cell& s, SymbolsProvider& provider)
{
    if (is_atom(f) || is_null(to_list(f))) return s;
    return cons_cell(car(f), append_cell(cdr(f), s, provider), provider);
}

inline DotPair make_empty_list(SymbolsProvider& provider)
{
    return DotPair(provider);
}

inline Cell make_empty_list_cell(SymbolsProvider& provider)
{
    return Cell(DotPair(provider));
}

inline DotPair make_list(std::initializer_list<Cell> l, SymbolsProvider& provider)
{
    DotPair result = make_empty_list(provider);
    for (auto it = rbegin(l); it != rend(l); ++it)
    {
        result = cons(*it, make_list_cell(std::move(result)), provider);
    }
    return result;
}

inline DotPair deep_copy_list(const DotPair& l)
{
    throw;
    //return DotPair();
}

inline bool is_null(const DotPair& dp)
{
    return std::empty(dp);
}


inline std::vector<Cell> copy_dpair_list_to_vector(const Cell& dp, bool skip_last_pair)
{
    return copy_dpair_list_to_vector(to_list(dp), skip_last_pair);
}

inline std::vector<Cell> copy_dpair_list_to_vector(const DotPair& dp, bool skip_last_pair)
{
    std::vector<Cell> result;
    for (const auto& cell : CarCdrConstIteration(dp)) {
        result.push_back(cell);
    }
    return result;
}

