#pragma once
#include "Cell.h"
#include "Symbol.h"
#include "DotPair.h"
#include "Number.h"

#include <vector>

enum class lambda_types : bool { lambda, nlambda };
enum class lambda_args_types : bool { spread, nospread};

struct lambda {
    lambda_types type = lambda_types::lambda;
    lambda_args_types arg_type = lambda_args_types::spread;
    Cell params;
    Cell body;
};

inline bool is_lambda(const lambda& l);
inline bool is_nlambda(const lambda& l);

inline bool is_spread(const lambda& l);
inline bool is_nospread(const lambda& l);

lambda make_lambda(lambda_types type, lambda_args_types arg_type, const Cell& params, Cell& body, SExprsFarm& farm);
Cell gen_cell(const lambda& l, SExprsFarm& farm);









inline bool is_lambda(const lambda& l)
{
    return l.type == lambda_types::lambda;
}

inline bool is_nlambda(const lambda& l)
{
    return l.type == lambda_types::nlambda;
}

inline bool is_spread(const lambda& l)
{
    return l.arg_type == lambda_args_types::spread;
}

inline bool is_nospread(const lambda& l)
{
    return l.arg_type == lambda_args_types::nospread;
}

