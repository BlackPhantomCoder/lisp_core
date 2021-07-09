#include "LambdaCell.h"


const lambda& LambdaCell::get() const
{
    return t_func;
}

LambdaCell::LambdaCell(lambda_types type, const lambda& func) :
    t_type(type),
    t_func(func)
{
}
LambdaCell::~LambdaCell() {
    t_func = {};
}

bool LambdaCell::is_lambda()const {
    return  (t_func.body.is_list() || t_func.body.is_atom()) && (t_type == lambda_types::lambda);
}

bool LambdaCell::is_nlambda()const {
    return  (t_func.body.is_list() || t_func.body.is_atom()) && (t_type == lambda_types::nlambda); 
}

LambdaCell make_lambda(std::vector<Cell>&& params, const Cell& body)
{
    return LambdaCell(lambda_types::lambda, lambda{ std::move(params), body });
}

LambdaCell make_lambda(const std::vector<Cell>& params, const Cell& body)
{
    return LambdaCell(lambda_types::lambda, lambda{ params, body });
}

LambdaCell make_nlambda(std::vector<Cell>&& params, const Cell& body)
{
    return LambdaCell(lambda_types::nlambda, lambda{ std::move(params), body });
}

LambdaCell make_nlambda(const std::vector<Cell>& params, const Cell& body)
{
    return LambdaCell(lambda_types::nlambda, lambda{ params, body });
}