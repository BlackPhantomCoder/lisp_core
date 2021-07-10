#pragma once
#include "Cell.h"
#include <vector>

enum class lambda_types : bool { lambda, nlambda };

struct lambda {
    //args symbs
    std::vector<Cell> params;
    Cell body;
};

class LambdaCell
{
public:
    LambdaCell(const LambdaCell& rh) = default;
    LambdaCell() = default;

    LambdaCell(lambda_types type, const lambda& func);
    ~LambdaCell();

    LambdaCell& operator=(const LambdaCell& rh) = default;


    bool is_lambda()const;
    bool is_nlambda()const;
    const lambda& get() const;

private:
    lambda_types t_type = lambda_types::lambda;
    lambda t_func;
};

LambdaCell make_lambda(std::vector<Cell>&& params, const Cell& body);
LambdaCell make_lambda(const std::vector<Cell>& params, const Cell& body);

LambdaCell make_nlambda(std::vector<Cell>&& params, const Cell& body);
LambdaCell make_nlambda(const std::vector<Cell>& params, const Cell& body);