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
    return   (t_type == lambda_types::lambda);
}

bool LambdaCell::is_nlambda()const {
    return  (t_type == lambda_types::nlambda);
}

LambdaCell make_lambda(std::vector<std::string> params, std::vector<Cell> body)
{
    return LambdaCell(lambda_types::lambda, lambda{ move(params), move(body) });
}

LambdaCell make_nlambda(std::vector<std::string> params, std::vector<Cell> body)
{
    return LambdaCell(lambda_types::nlambda, lambda{ move(params), move(body) });
}
