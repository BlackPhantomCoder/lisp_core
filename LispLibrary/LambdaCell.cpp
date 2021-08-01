#include "LambdaCell.h"
#include "Funcs.h"
#include "CoreEnv.h"
#include "Cell.h"
using namespace std;

const lambda& LambdaCell::get() const
{
    return t_func;
}

LambdaCell::LambdaCell(lambda_types type, lambda_args_types args_type, lambda&& func) :
    t_type(type),
    t_func(move(func)),
    t_args_type(args_type)
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

bool LambdaCell::is_spread() const
{
    return (t_args_type == lambda_args_types::spread);
}

bool LambdaCell::is_no_spread() const
{
    return (t_args_type == lambda_args_types::nospread);
}

lambda_args_types LambdaCell::get_args_type() const
{
    return t_args_type;
}

lambda_types LambdaCell::get_lambda_type() const
{
    return t_type;
}

LambdaCell make_lambda(lambda_types type, lambda_args_types arg_type, std::vector<Symbol> params, Cell::olist body)
{
    return LambdaCell(type, arg_type, lambda{ move(params), move(body) });
}

std::string to_string(const LambdaCell& fnc)
{
    if (fnc.is_lambda()) {
        const auto& lambda = fnc.get();
        string str = "(";
        str += CoreData::lambda_str;

        str += " ";
        if (fnc.is_spread()) {
            str += "(";
            if (!lambda.params.empty()) {
                for (const auto& name : lambda.params) {
                    str += name.to_string();
                    str += " ";
                }
                str.erase(str.size() - 1);
            }
            str += ")";
        }
        else if (fnc.is_no_spread()) {
            str += lambda.params[0].to_string();
        }
        else {
            throw "to_string: unknown spread type";
        }
        str +=" ";


        if (lambda.body.size()) {
            str += to_string(lambda.body[0]);
        }
        else {
            str += to_string(make_list(lambda.body));
        }
        str += ")";
        return str;
    }
    else if (fnc.is_nlambda()) {
        const auto& lambda = fnc.get();
        string str = "(";
        str += CoreData::nlambda_str;

        str += " ";
        if (fnc.is_spread()) {
            str += "(";
            if (!lambda.params.empty()) {
                for (const auto& name : lambda.params) {
                    str += name.to_string();
                    str += " ";
                }
                str.erase(str.size() - 1);
            }
            str += ")";
        }
        else  if(fnc.is_no_spread()) {
            str += lambda.params[0].to_string();
        }
        else {
            throw "to_string: unknown spread type";
        }
        str += " ";

        if (lambda.body.size()) {
            str += to_string(lambda.body[0]);
        }
        else {
            str += to_string(make_list(lambda.body));
        }
        str += ")";
        return str;
    }
    else {
        throw "to_string ...?";
    }
}
