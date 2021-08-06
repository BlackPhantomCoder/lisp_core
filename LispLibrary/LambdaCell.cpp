#include "LambdaCell.h"
#include "Funcs.h"
#include "CoreEnv.h"
#include "Cell.h"
using namespace std;


std::string to_string(const lambda& fnc)
{
    if (is_lambda(fnc)) {
        string str = "(";
        str += CoreData::lambda_str;

        str += " ";
        if (is_spread(fnc)) {
            str += "(";
            if (!fnc.params.empty()) {
                for (const auto& name : fnc.params) {
                    str += name.to_string();
                    str += " ";
                }
                str.erase(str.size() - 1);
            }
            str += ")";
        }
        else if (is_nospread(fnc)) {
            str += fnc.params[0].to_string();
        }
        else {
            throw "to_string: unknown spread type";
        }
        str +=" ";


        if (!empty(fnc.body)) {
            str += to_string(car(fnc.body));
        }
        else {
            str += to_string(fnc.body);
        }
        str += ")";
        return str;
    }
    else if (is_nlambda(fnc)) {
        string str = "(";
        str += CoreData::nlambda_str;

        str += " ";
        if (is_spread(fnc)) {
            str += "(";
            if (!fnc.params.empty()) {
                for (const auto& name : fnc.params) {
                    str += name.to_string();
                    str += " ";
                }
                str.erase(str.size() - 1);
            }
            str += ")";
        }
        else  if(is_nospread(fnc)) {
            str += fnc.params[0].to_string();
        }
        else {
            throw "to_string: unknown spread type";
        }
        str += " ";

        if (!empty(fnc.body)) {
            str += to_string(car(fnc.body));
        }
        else {
            str += to_string(fnc.body);
        }
        str += ")";
        return str;
    }
    else {
        throw "to_string ...?";
    }
}
