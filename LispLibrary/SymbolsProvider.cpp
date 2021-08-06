#include "SymbolsProvider.h"
#include "DotPair.h"
#include "Funcs.h"
using namespace CoreData;

std::vector<Symbol> make_symbols(Symbols& symbols) {
    std::vector<Symbol> v;
    v.reserve(bifuncs_count + nbifuncs_count);
    for (const auto& [name, fnc] : SymbolsProvider::bifuncs_arr) {
        v.emplace_back(make_symbol(name, symbols));
    }
    for (const auto& [name, fnc] : SymbolsProvider::nbifuncs_arr) {
        v.emplace_back(make_symbol(name, symbols));
    }
    return v;
}


std::unordered_map<Symbol, CoreData::bifunc> make_bifuncs(Symbols& symbols) {
    std::unordered_map<Symbol, CoreData::bifunc> v;
    for (const auto& [name, fnc] : SymbolsProvider::bifuncs_arr) {
        v.emplace(make_symbol(name, symbols), fnc);
    }
    return v;
}

std::unordered_map<Symbol, CoreData::bifunc> make_nbifuncs(Symbols& symbols) {
    std::unordered_map<Symbol, CoreData::bifunc> v;
    for (const auto& [name, fnc] : SymbolsProvider::nbifuncs_arr) {
        v.emplace(make_symbol(name, symbols), fnc);
    }
    return v;
}



const bifuncs_array SymbolsProvider::bifuncs_arr = bifunc_setup();
const nbifuncs_array SymbolsProvider::nbifuncs_arr = nbifunc_setup();

SymbolsProvider::SymbolsProvider():
    nil_symbol(make_symbol(nil_str, symbols)),
    T_symbol(make_symbol(T_str, symbols)),
    read_up_case_symbol(make_symbol(read_up_case_str, symbols)),

    lambda_symbol(make_symbol(lambda_str, symbols)),
    nlambda_symbol(make_symbol(nlambda_str, symbols)),

    nil_list(make_symbol_cell(nil_str, symbols)),
    nil(make_empty_list_cell(*this)),
    T(make_symbol_cell(T_str, symbols))
    //t_some_symbols(make_symbols(symbols))
{
    bifuncs = make_bifuncs(symbols);
    nbifuncs = make_nbifuncs(symbols);
}
