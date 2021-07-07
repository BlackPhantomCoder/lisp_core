#include "FuncCell.h"

lambda_cell make_lambda(std::vector<cell>&& params, const cell& body)
{
    return lambda_cell(lambda_types::lambda, ilambda(std::move(params), body));
}

lambda_cell make_lambda(const std::vector<cell>& params, const cell& body)
{
    return lambda_cell(lambda_types::lambda, ilambda(params, body));
}

lambda_cell make_nlambda(std::vector<cell>&& params, const cell& body)
{
    return lambda_cell(lambda_types::nlambda, ilambda(std::move(params), body));
}

lambda_cell make_nlambda(const std::vector<cell>& params, const cell& body)
{
    return lambda_cell(lambda_types::nlambda, ilambda(params, body));
}

bifunc_cell make_bifunc(bifunc_cell::bifunc2 func){
    return bifunc_cell(bifunc_types::bifunc, (void*)func);
}

bifunc_cell make_nbifunc(bifunc_cell::nbifunc2 func){
    return bifunc_cell(bifunc_types::nbifunc, (void*)func);
}
