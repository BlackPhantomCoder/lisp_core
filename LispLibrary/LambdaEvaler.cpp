#include "LambdaEvaler.h"
#include "CoreEnv.h"

using namespace std;

LambdaEvaler::LambdaEvaler(CoreEnvironment* env):
    t_env(env)
{
}

void LambdaEvaler::push_lambda(
    const LambdaCell& fnc, Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, bool forse_eval_args
) 
{
    t_frames.push({ fnc.get(), fnc.get_args_type(), beg_it , end_it ,forse_eval_args });
}

Cell LambdaEvaler::pop_eval()
{
    eval_args();
    if (t_frames.top().buf.empty()) {
        Cell result = 
            env_eval_direct_bifunc(
                &CoreEnvironment::nbifunc_progn, begin(t_frames.top().fnc.body), end(t_frames.top().fnc.body), t_env
            );
        t_frames.pop();
        return result;
    }
    else {
        t_env->t_envs.push(move(t_frames.top().buf));
        Cell result = 
            env_eval_direct_bifunc(
                &CoreEnvironment::nbifunc_progn, begin(t_frames.top().fnc.body), end(t_frames.top().fnc.body), t_env
            );
        t_env->t_envs.pop();
        t_frames.pop();
        return result;
    }
}

void LambdaEvaler::clear()
{
	while (!t_frames.empty()) {
		t_frames.pop();
	}
}

void LambdaEvaler::eval_args()
{
    Cell::olist::const_iterator it = t_frames.top().beg_it;
    if (t_frames.top().arg_type == lambda_args_types::spread) {
        if (t_frames.top().eval_args) {
            for (auto arg_it = begin(t_frames.top().fnc.params); arg_it != end(t_frames.top().fnc.params); ++arg_it) {
                if (it == t_frames.top().end_it) {
                    t_frames.top().buf.emplace(*arg_it, t_env->t_symbols.nil);
                }
                else {
                    t_frames.top().buf.emplace(*arg_it, t_env->eval_quote(*it));
                    ++it;
                }
            }
            for (; it != t_frames.top().end_it; ++it) {
                t_env->eval_quote(*it);
            }
        }
        else {
            for (auto arg_it = begin(t_frames.top().fnc.params); arg_it != end(t_frames.top().fnc.params); ++arg_it) {
                if (it == t_frames.top().end_it) {
                    t_frames.top().buf.emplace(*arg_it, t_env->t_symbols.nil);
                }
                else {
                    t_frames.top().buf.emplace(*arg_it, *it);
                    ++it;
                }
            }
        }
    }
    else if(t_frames.top().arg_type == lambda_args_types::nospread){
        if (t_frames.top().fnc.params.empty()) throw "LambdaEvaler::eval_args: nospread without arg";
        Cell::olist params_list;
        if (t_frames.top().eval_args) {
            for (; it != t_frames.top().end_it; ++it) {
                params_list.emplace_back(t_env->eval_quote(*it));
            }
            t_frames.top().buf.emplace(t_frames.top().fnc.params[0], make_list(move(params_list)));
        }
        else {
            for (; it != t_frames.top().end_it; ++it) {
                params_list.emplace_back(*it);
            }
            t_frames.top().buf.emplace(t_frames.top().fnc.params[0], make_list(move(params_list)));
        }
    }
    else {
        throw "LambdaEvaler::eval_args: unknown args_type";
    }
}
