#include "FuncsStorage.h"
#include "CoreData.h"
#include "Funcs.h"

#include <vector>
using namespace CoreData;
using namespace std;

void make_bifuncs(FuncsStorage::mp& mp, SExprsFarm& farm) {
    for (const auto& [name, fnc] : FuncsStorage::bifuncs_arr) {
        mp.emplace(farm.make_symbol(name), FuncsStorage::bifunc{ fnc });
    }
}

void  make_nbifuncs(FuncsStorage::mp& mp, SExprsFarm& farm) {
    for (const auto& [name, fnc] : FuncsStorage::nbifuncs_arr) {
        mp.emplace(farm.make_symbol(name), FuncsStorage::nbifunc{ fnc });
    }
}

const bifuncs_array FuncsStorage::bifuncs_arr = bifunc_setup();
const nbifuncs_array FuncsStorage::nbifuncs_arr = nbifunc_setup();

FuncsStorage::FuncsStorage(SExprsFarm& farm):
    t_farm(farm)
{
    make_bifuncs(t_funcs, farm);
    make_nbifuncs(t_funcs, farm);
}

void FuncsStorage::add_lambda(const Symbol& symbol, const lambda& cell)
{
    t_funcs.emplace(symbol, cell);
}

void FuncsStorage::add_lambda(const Symbol& symbol, lambda&& cell)
{
    t_funcs.emplace(symbol, move(cell));
}

std::variant<std::reference_wrapper<lambda>, FuncsStorage::bifunc, FuncsStorage::nbifunc, std::monostate> FuncsStorage::find(
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
        else {
            throw "";
        }
    }
    return monostate{};
}
