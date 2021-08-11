#include "LambdaCell.h"
#include "Funcs.h"
#include "CoreEnv.h"
#include "Cell.h"
using namespace std;


Cell gen_cell(const lambda& fnc, SExprsFarm& farm)
{
    //if (is_macro(fnc)) {

    //}
    std::vector<Cell> lst;
    lst.reserve(3);
    lst.push_back((is_lambda(fnc)) ? farm.lambda_symbol : farm.nlambda_symbol);
    lst.push_back((is_spread(fnc)) ? fnc.params : car(fnc.params));

    // tree copy or not?
    auto copy = tree_copy(fnc.body, farm);
    CarCdrConstIteration iteration(copy, farm);
    for (const auto& c : iteration) {
        lst.push_back(c);
    }
    return farm.make_list_cell(begin(lst), end(lst));
}
