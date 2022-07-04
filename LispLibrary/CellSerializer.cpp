#include "CellSerializer.h"
#include "Funcs.h"
#include "CarCdrIterator.h"
#include "CoreEnv.h"

#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;
using namespace nlohmann;
using namespace CoreData;

void to_json(json& j, const Cell& cell) {

    auto t = type(cell);
    switch (t)
    {
    case cell_type::numb: 
        {
            auto& n = to_number(cell);
            if (is_integer(n)) {
                j = to_integer(n).to_string() + "0";
            }
            else if (is_real(n)) {
                j = to_real(n);
            }
            else {
                throw std::logic_error("to_json");
            }
        }
        break;
    case cell_type::symbol:
        j = string(to_symbol(cell).to_string()) + "1";
        
        break;
    case cell_type::list:
        j = vector<Cell>{ begin(cell), end(cell) };
        break;
    default:
        throw logic_error("CellSerializer::out");
    }
}


Cell from_json(const nlohmann::json& j, CoreEnvironment& env) {

    if (j.is_array()) {
        auto cvec = vector<Cell>();
        cvec.reserve(j.size());
        for (const auto& jelem : j) {
            cvec.push_back(::from_json(jelem, env));
        }
        return env.farm().make_list_cell(begin(cvec), end(cvec));
    }
    else if (j.is_string()) {
        auto str = j.get<string>();
        auto type = str.back();
        str.pop_back();
        if (type == '0') {
            return env.farm().make_number_cell(BigInt(str));
        }
        else {
            return env.farm().make_symbol_cell(str);
        }
    }
    else if (j.is_number_float()) {
        return env.farm().make_number_cell(j.get<double>());
    }
    else {
        throw logic_error("from_json");
    }
}





CellSerializer::CellSerializer(CoreEnvironment& env):
    t_env(env)
{
}

void CellSerializer::out(nlohmann::json& j, const lambda& l)
{
    auto types = int(l.type) + (int(l.arg_type) << 1);
    j.emplace_back(types);
    out(j.emplace_back(), l.params);
    out(j.emplace_back(), l.body);
}

void CellSerializer::out(nlohmann::json& j, const Cell& cell)
{
    ::to_json(j, cell);
}

lambda CellSerializer::in_lambda(const nlohmann::json& j)
{
    auto types = j.at(0).get<int>();
    return lambda{
        lambda_types(types),
        lambda_args_types(types >> 1),
        in(j.at(1)),
        in(j.at(2))
    };
}

Cell CellSerializer::in(const nlohmann::json& j)
{
    return ::from_json(j, t_env);
}

