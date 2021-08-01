#pragma once
#include "Cell.h"
#include <vector>

enum class lambda_types : bool { lambda, nlambda };
enum class lambda_args_types : bool { spread, nospread};

struct lambda {
    //args symbs
    std::vector<Symbol> params;
    Cell::olist body;
};

class LambdaCell
{
public:
    LambdaCell(const LambdaCell& rh) = default;
    LambdaCell() = default;

    LambdaCell(lambda_types type, lambda_args_types args_type, lambda&& func);
    ~LambdaCell();

    LambdaCell& operator=(const LambdaCell& rh) = default;


    bool is_lambda()const;
    bool is_nlambda()const;
    bool is_spread()const;
    bool is_no_spread()const;

    lambda_args_types get_args_type()const;
    lambda_types get_lambda_type()const;

    const lambda& get() const;

private:
    lambda_types t_type = lambda_types::lambda;
    lambda_args_types t_args_type = lambda_args_types::nospread;
    lambda t_func;
};

LambdaCell make_lambda(lambda_types type, lambda_args_types arg_type, std::vector<Symbol> params, Cell::olist body);
std::string to_string(const LambdaCell& fnc);