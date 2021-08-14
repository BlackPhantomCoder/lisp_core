#include "FuncsStorage.h"
#include "CoreData.h"
#include "Funcs.h"

#include <vector>
using namespace CoreData;
using namespace std;

static const bifuncs_array bifuncs_arr = bifunc_setup();
static const nbifuncs_array nbifuncs_arr = nbifunc_setup();
static const special_bifuncs_array special_bifuncs_arr = special_bifunc_setup();
static const special_nbifuncs_array special_nbifuncs_arr = special_nbifunc_setup();

void make_bifuncs(FuncsStorage::mp& mp, SExprsFarm& farm) {
    for (const auto& [name, fnc] : bifuncs_arr) {
        mp.emplace(farm.make_symbol(name), FuncsStorage::bifunc{ fnc });
    }
}

void  make_nbifuncs(FuncsStorage::mp& mp, SExprsFarm& farm) {
    for (const auto& [name, fnc] : nbifuncs_arr) {
        mp.emplace(farm.make_symbol(name), FuncsStorage::nbifunc{ fnc });
    }
}

void  make_special_bifuncs(FuncsStorage::mp& mp, SExprsFarm& farm) {
    for (const auto& [name, fnc] : special_bifuncs_arr) {
        mp.emplace(farm.make_symbol(name), fnc);
    }
}

void  make_special_nbifuncs(FuncsStorage::mp& mp, SExprsFarm& farm) {
    for (const auto& [name, fnc] : special_nbifuncs_arr) {
        mp.emplace(farm.make_symbol(name), fnc);
    }
}

FuncsStorage::FuncsStorage(SExprsFarm& farm):
    t_farm(farm)
{
    make_bifuncs(t_funcs, farm);
    make_nbifuncs(t_funcs, farm);
    make_special_bifuncs(t_funcs, farm);
    make_special_nbifuncs(t_funcs, farm);
}

void FuncsStorage::add_lambda(const Symbol& symbol, const lambda& cell)
{
    t_funcs.emplace(symbol, cell);
}

void FuncsStorage::add_lambda(const Symbol& symbol, lambda&& cell)
{
    t_funcs.emplace(symbol, move(cell));
}

std::variant<
    std::reference_wrapper<lambda>,
    FuncsStorage::bifunc,
    FuncsStorage::nbifunc,
    CoreData::special_bifunc_make,
    CoreData::special_nbifunc_make,
    std::monostate
> FuncsStorage::find(
    const Symbol& symbol
)
{
    if (auto it = t_funcs.find(symbol); it != end(t_funcs)) {
        if (holds_alternative<lambda>(it->second)) {
            return get<lambda>(it->second);
        }
        else if (holds_alternative<bifunc>(it->second)) {
            return get<bifunc>(it->second);
        }
        else if (holds_alternative<nbifunc>(it->second)) {
            return get<nbifunc>(it->second);
        }
        else if (holds_alternative<special_bifunc_make>(it->second)) {
            return get<special_bifunc_make>(it->second);
        }
        else if (holds_alternative<special_nbifunc_make>(it->second)) {
            return get<special_nbifunc_make>(it->second);
        }
        else {
            throw "";
        }
    }
    return monostate{};
}
