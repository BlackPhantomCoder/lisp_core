#pragma once
#include "Cell.h"
#include <list>
#include <string>
#include <iostream>
#include "PublicCoreEnvironmentProvider.h"
#include "SymbolsProvider.h"

std::string to_string(const Cell& exp);
std::string to_string(const DPair& exp);
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

inline bool is_alambda_form(const DPair& arg, SymbolsProvider& provider);
inline bool is_lambda_form(const DPair& arg, SymbolsProvider& provider);
inline bool is_nlambda_form(const DPair& arg, SymbolsProvider& provider);

inline bool is_alambda_symbol(const Symbol& arg, SymbolsProvider& provider);










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
    if (!is_list(to_list(arg)[0])) return false;
    if (is_null(to_list(arg)[0], provider)) return true;
    if (!is_symbol(to_list(to_list(arg)[0])[0])) return true;
    if (is_alambda_symbol(to_symbol(to_list(to_list(arg)[0])[0]), provider)) return false;
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

inline bool is_alambda_form(const DPair& arg, SymbolsProvider& provider)
{
    return !std::empty(arg) && is_symbol(arg[0]) && is_alambda_symbol(to_symbol(arg[0]), provider);       
}

inline bool is_lambda_form(const DPair& arg, SymbolsProvider& provider)
{
    return !std::empty(arg) && is_symbol(arg[0]) && to_symbol(arg[0]) == provider.lambda_symbol;
}

inline bool is_nlambda_form(const DPair& arg, SymbolsProvider& provider)
{
    return !std::empty(arg) && is_symbol(arg[0]) && to_symbol(arg[0]) == provider.nlambda_symbol;
}

inline bool is_alambda_symbol(const Symbol& arg, SymbolsProvider& provider)
{
    return arg == provider.lambda_symbol || arg == provider.nlambda_symbol;
}
