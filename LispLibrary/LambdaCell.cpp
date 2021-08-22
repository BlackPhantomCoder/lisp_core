#include "LambdaCell.h"
#include "Funcs.h"
#include "CoreEnv.h"
#include "Cell.h"
using namespace std;

lambda make_lambda(lambda_types type, lambda_args_types arg_type, const Cell& params, Cell& body, SExprsFarm& farm)
{
    return { type, arg_type, tree_copy(params, farm), body };
}

Cell gen_cell(const lambda& fnc, SExprsFarm& farm)
{
    std::vector<Cell> lst;
    lst.reserve(3);

    lst.push_back((is_lambda(fnc)) ? farm.lambda_symbol() : farm.nlambda_symbol());
    lst.push_back((is_spread(fnc)) ? fnc.params : car(fnc.params));

    // tree copy or not?
    auto copy = tree_copy(fnc.body, farm);
    for (const auto& c : copy) {
        lst.push_back(c);
    }
    return farm.make_list_cell(begin(lst), end(lst));
}
