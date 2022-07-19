#include "CoreFuncs.h"
#include "CoreEnv.h"
#include "Funcs.h"
#include "ArgsCounter.h"
using namespace std;


CoreFuncs::CoreFuncs(CoreEnvironment& env):
    t_env(env)
{
}

lambda CoreFuncs::make_spread_lambda_form(
    lambda_types lambda_type,
    const Cell& params,
    Cell& body
)
{
    return  make_lambda(lambda_type, lambda_args_types::spread, params, body, t_env.farm());
}

lambda CoreFuncs::make_nospread_lambda_form(
    lambda_types lambda_type,
    const Symbol& param,
    Cell& body
)
{
    return  make_lambda(lambda_type, lambda_args_types::nospread, t_env.farm().make_symbol_cell(param), body, t_env.farm());
}

macro CoreFuncs::make_spread_macro_form(
    const Cell& params,
    const Cell& body
)
{
    auto buf = tree_copy(params, t_env.farm());
    return  make_macro(lambda_args_types::spread, buf, body, t_env.farm());
}

macro CoreFuncs::make_nospread_macro_form(
    const Symbol& param,
    const Cell& body
)
{
    auto buf = t_env.farm().make_symbol_cell(param);
    return  make_macro(lambda_args_types::nospread, buf, body, t_env.farm());
}

lambda CoreFuncs::make_lambda_form(Cell& lst)
{
    if (is_null(cdr(lst))) throw "get_lambda_form error";
    auto& second = car(cdr(lst));
    if (is_list(second)) {
        return make_spread_lambda_form(
            (to_symbol(car(lst)) == to_symbol(t_env.farm().lambda_symbol())) ? lambda_types::lambda : lambda_types::nlambda,
            second,
            cdr(cdr(lst))
        );
    }
    else if (is_symbol(second)) {
        return make_nospread_lambda_form(
            (to_symbol(car(lst)) == to_symbol(t_env.farm().lambda_symbol())) ? lambda_types::lambda : lambda_types::nlambda,
            to_symbol(second),
            cdr(cdr(lst))
        );
    }
    throw "get_lambda_form error";
}


void CoreFuncs::set_value(const Cell& name, const Cell& val)
{
    if (!is_symbol(name)) {
        if (!is_null(name)) {
            throw "bifunc_set error";
        }
    }
    else {
        const auto& symb = to_symbol(name);
        //cout << env->output_control().to_string(name) << " = " << env->output_control().to_string(val) << endl;
        t_env.envs().set(symb, val);

        //special vars
        if (symb == to_symbol(t_env.farm().read_up_case_symbol())) {
            bool val_b = !is_null(val);
            t_env.output_control().set_read_upcase(val_b);
        }
    }
}

Cell CoreFuncs::eval_atom(const Cell& atom) {
    if (is_symbol(atom)) {
        return eval_symbol(atom);
    }
    return atom;
}

Cell CoreFuncs::eval_symbol(const Cell& symbol) {
    if (is_null(symbol)) {
        return t_env.farm().nil();
    }
    if (auto val = t_env.envs().get(to_symbol(symbol)); !empty(val)) {
        return val;
    }
    return symbol;
}
