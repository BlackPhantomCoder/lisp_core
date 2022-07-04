#include "FuncsStorage.h"
#include "CoreData.h"
#include "Funcs.h"
#include "CoreEnv.h"

#include <json/include/json.hpp>
#include <vector>
using namespace CoreData;
using namespace std;
using namespace nlohmann;

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

FuncsStorage::FuncsStorage(CoreEnvironment& env) :
    t_env(env)
{

}

void FuncsStorage::init(std::optional<std::reference_wrapper<nlohmann::json>> state)
{
    if (state) {
        load_state(*state);
    }
    else {
        t_load_bifuncs();
    }
}

void FuncsStorage::set_func(const Symbol& symbol, const lambda& cell)
{
    t_funcs[symbol] = cell;
}

void FuncsStorage::set_func(const Symbol& symbol, const macro& cell)
{
    t_funcs[symbol] = cell;
}

void FuncsStorage::set_func(const Symbol& symbol, lambda&& cell)
{
    t_funcs[symbol] = move(cell);
}

void FuncsStorage::set_func(const Symbol& symbol, macro&& cell)
{
    t_funcs[symbol] = move(cell);
}

std::optional<std::reference_wrapper<const FuncsStorage::data>> FuncsStorage::find(const Symbol& symbol)
{
    if (auto it = t_funcs.find(symbol); it != end(t_funcs)) {
        return { it->second };
    }
    return nullopt;
}

enum class state_type : char{ macro, lambda};

void FuncsStorage::save_state(nlohmann::json& j)
{
    auto& cs = t_env.cserial();
    auto& farm = t_env.farm();

    for (const auto& [symbol, func] : t_funcs) {
        auto& elem = j.emplace_back();

        if (holds_alternative<macro>(func)) {
            elem.emplace_back(string(symbol.to_string()) + char(char(state_type::macro) + 48));
            auto& jfunc = elem.emplace_back();
            {
                auto& jparams = jfunc.emplace_back();
                for (const auto& param : get<macro>(func).params) {
                    auto& jparam = jparams.emplace_back();
                    cs.out(jparam.emplace_back(), param.first);
                    auto& jbools = jparam.emplace_back();
                    for (const auto& b : param.second) {
                        jbools.emplace_back(int(b));
                    }
                }
                
            }
            cs.out(jfunc.emplace_back(), get<macro>(func).body);
        }
        else if (holds_alternative<lambda>(func)) {
            elem.emplace_back(string(symbol.to_string()) + char(char(state_type::lambda) + 48));
            auto& jfunc = elem.emplace_back();

            cs.out(jfunc, get<lambda>(func));
        }
        else {
            j.erase(prev(end(j)));
        }
    }
}

void FuncsStorage::load_state(const nlohmann::json& j)
{
    auto& cs = t_env.cserial();
    t_funcs.clear();

    t_load_bifuncs();

    for (auto& elem : j) {
        auto& func = elem.at(1);
        auto str = elem.at(0).get<string>();
        auto type = state_type(str.back()- 48);
        str.pop_back();
        auto symbol = t_env.farm().make_symbol(move(str));
        switch (type)
        {
        case state_type::macro:
        {
                auto& macro = get<::macro>((*t_funcs.emplace(symbol, ::macro{}).first).second);
                for (const auto& jparam : func.at(0)) {
                    auto& jbools = jparam.at(1);
                    auto bools = vector<bool>();
                    bools.reserve(jbools.size());
                    for(const int& b : jbools){
                        bools.push_back(bool(b));
                    }

                    macro.params.emplace_back(cs.in(jparam.at(0)), move(bools));
                }
                macro.body = cs.in_lambda(func.at(1));
            }
            break;
        case state_type::lambda:
            {
                t_funcs[symbol] = cs.in_lambda(func);
            }
            break;
        default:
            throw logic_error("FuncsStorage::load_state");
        }
    }
}

void FuncsStorage::t_load_bifuncs()
{
    make_bifuncs(t_funcs, t_env.farm());
    make_nbifuncs(t_funcs, t_env.farm());
    make_special_bifuncs(t_funcs, t_env.farm());
    make_special_nbifuncs(t_funcs, t_env.farm());
}
