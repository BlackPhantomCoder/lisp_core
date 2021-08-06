#pragma once
#include "Cell.h"
#include "DotPair.h"
#include <vector>

enum class lambda_types:bool { lambda, nlambda /*, macro*/ };
enum class lambda_args_types : bool { spread, nospread};

struct lambda {
    lambda_types type;
    lambda_args_types arg_type;
    //args symbs
    std::vector<Symbol> params;
    //must be DPair
    DotPair body;
};

inline bool is_lambda(const lambda& l);
inline bool is_nlambda(const lambda& l);
//inline bool is_macro(const lambda& l);

inline bool is_spread(const lambda& l);
inline bool is_nospread(const lambda& l);

inline lambda make_lambda(lambda_types type, lambda_args_types arg_type, const std::vector<Symbol>& params, const DotPair& body);
inline lambda make_lambda(lambda_types type, lambda_args_types arg_type, std::vector<Symbol>&& params, const DotPair& body);
inline lambda make_lambda(lambda_types type, lambda_args_types arg_type, const std::vector<Symbol>& params, DotPair&& body);
inline lambda make_lambda(lambda_types type, lambda_args_types arg_type, std::vector<Symbol>&& params, DotPair&& body);
std::string to_string(const lambda& fnc);









inline bool is_lambda(const lambda& l)
{
    return l.type == lambda_types::lambda;
}

inline bool is_nlambda(const lambda& l)
{
    return l.type == lambda_types::nlambda;
}

//inline bool is_macro(const lambda& l)
//{
//    return l.type == lambda_types::macro;
//}

inline bool is_spread(const lambda& l)
{
    return l.arg_type == lambda_args_types::spread;
}

inline bool is_nospread(const lambda& l)
{
    return l.arg_type == lambda_args_types::nospread;
}

inline lambda make_lambda(lambda_types type, lambda_args_types arg_type, const std::vector<Symbol>& params, const DotPair& body)
{
    return { type, arg_type, params, body };
}

inline lambda make_lambda(lambda_types type, lambda_args_types arg_type, std::vector<Symbol>&& params, const DotPair& body)
{
    return  { type, arg_type, move(params), body };
}

inline lambda make_lambda(lambda_types type, lambda_args_types arg_type, const std::vector<Symbol>& params, DotPair&& body)
{
    return  { type, arg_type, params, std::move(body) };
}

inline lambda make_lambda(lambda_types type, lambda_args_types arg_type, std::vector<Symbol>&& params, DotPair&& body)
{
    return  { type, arg_type, move(params), std::move(body) };
}