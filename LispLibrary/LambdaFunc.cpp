#include "LambdaFunc.h"
#include "CoreEnv.h"
#include "Bifuncs.h"
using namespace std;

ALambdaFunc::ALambdaFunc(
    CoreEnvironment& env,
    Cell params,
    Cell body,
    CarCdrIterator beg_it,
    CarCdrIterator end_it,
    eval_types type,
    spread_types stype,
    bool forse_noeval_args
):
    Func(env),
    t_params_list(params),
    t_body(body),
    t_spread_type(stype)
{
    if (type == eval_types::no_eval || forse_noeval_args) {
        t_args = pair{ beg_it , end_it };
    }
    else {
        t_args_eval_func = make_fnc<EvalQuoteRange>(t_env(), beg_it, end_it);
    }
}

void ALambdaFunc::t_init_before_args()
{
    if (t_args_eval_func) {
        t_eval_next(move(*t_args_eval_func));
        t_args_eval_func.reset();
    }
    return t_cycle();
}

void ALambdaFunc::t_init_after_args()
{
    return t_eval_next(
        make_fnc<ProgN>(
            t_env(),
            begin(t_body),
            end(t_body)
            )
    );
}

bool ALambdaFunc::t_eval_args()
{
    if(holds_alternative<monostate>(t_args)) t_args = t_last_eval_val();
    t_env().t_envs.push(t_create_frame());
    return true;
}

void ALambdaFunc::t_internal_execute()
{
    t_env().t_envs.pop();
    return t_return(t_last_eval_val());
}

CellEnvironment::frame ALambdaFunc::t_create_frame()
{
    if (holds_alternative<monostate>(t_args)) throw "";
    CellEnvironment::frame buf;
    if (t_spread_type == spread_types::spread) {
        CarCdrIterator arg_it;
        CarCdrIterator arg_end_it;
        if (holds_alternative<Cell>(t_args)) {
            arg_it = begin(get<Cell>(t_args));
            arg_end_it = end(get<Cell>(t_args));
        }
        else {
            arg_it = get<pair<CarCdrIterator, CarCdrIterator>>(t_args).first;
            arg_end_it = get<pair<CarCdrIterator, CarCdrIterator>>(t_args).second;
        }
        buf.first = { begin(t_params_list), end(t_params_list) };
        buf.second = { arg_it, arg_end_it };
    }
    else {
        if (holds_alternative<Cell>(t_args)) {
            t_temp = t_env().t_farm.make_list_cell({ get<Cell>(t_args) });
            buf.first = { begin(t_params_list), end(t_params_list) };
            buf.second = { begin(t_temp), end(t_temp) };
        }
        else {
            t_temp = t_env().t_farm.make_list_cell(
                get<pair<CarCdrIterator, CarCdrIterator>>(t_args).first,
                get<pair<CarCdrIterator, CarCdrIterator>>(t_args).second
            );
            buf.first = { begin(t_params_list), end(t_params_list) };
            buf.second = { begin(t_temp), end(t_temp) };
        }
      
    }
    return buf;
}