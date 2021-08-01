#include "SymbolsProvider.h"
using namespace CoreData;

std::vector<Symbol> make_symbols(Symbols& symbols) {
    std::vector<Symbol> v;
    v.reserve(bifuncs_count + nbifuncs_count);
    for (const auto& [name, fnc] : SymbolsProvider::bifuncs) {
        v.emplace_back(make_symbol(name, symbols));
    }
    for (const auto& [name, fnc] : SymbolsProvider::nbifuncs) {
        v.emplace_back(make_symbol(name, symbols));
    }
    return v;
}


const bifuncs_array SymbolsProvider::bifuncs = bifunc_setup();
const nbifuncs_array SymbolsProvider::nbifuncs = nbifunc_setup();

SymbolsProvider::SymbolsProvider():
    nil_symbol(make_symbol(nil_str, symbols)),
    T_symbol(make_symbol(T_str, symbols)),
    read_up_case_symbol(make_symbol(read_up_case_str, symbols)),

    lambda_symbol(make_symbol(lambda_str, symbols)),
    nlambda_symbol(make_symbol(nlambda_str, symbols)),

    nil_list(make_symbol_cell(nil_str, symbols)),
    nil(make_list({})),
    T(make_symbol_cell(T_str, symbols)),
    t_some_symbols(make_symbols(symbols))
{
}
