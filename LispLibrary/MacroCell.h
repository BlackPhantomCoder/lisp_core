#pragma once
#include "LambdaCell.h"
#include <vector>

struct macro {
    //name and link (link == sequence car/cdr (true == car/ false == cdr) (except first car))
    std::vector<std::pair<Cell, std::vector<bool>>> params;
    lambda body;
};

Cell link_params(const macro& m, CarCdrConstIterator b, CarCdrConstIterator e, SExprsFarm& farm);
Cell link_params(const macro& m, const Cell& param_list, SExprsFarm& farm);
macro make_macro(lambda_args_types arg_type, const Cell& params, const Cell& body, SExprsFarm& farm);
Cell gen_cell(const macro& l, SExprsFarm& farm);