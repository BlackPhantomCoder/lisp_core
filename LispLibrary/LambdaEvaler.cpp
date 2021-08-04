#include "LambdaEvaler.h"
#include "CoreEnv.h"

using namespace std;

LambdaEvaler::LambdaEvaler(CoreEnvironment* env):
    ArgsOnStackEvaler(env)
{
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

inline void LambdaEvaler::eval_args()
{
    //DPair::const_iterator it = t_frames.top().beg_it;
    if (is_spread(t_frames.top().fnc)) {
        if (!t_frames.top().forse_nospread_args) {
            for (auto arg_it = begin(t_frames.top().fnc.params); arg_it != end(t_frames.top().fnc.params); ++arg_it) {
                if (t_frames.top().beg_it == t_frames.top().end_it) {
                    t_frames.top().buf.emplace_back(*arg_it, t_env->t_symbols.nil);
                }
                else {
                    t_frames.top().buf.emplace_back(*arg_it, t_env->eval_quote(*t_frames.top().beg_it));
                    ++t_frames.top().beg_it;
                }
            }
            for (; t_frames.top().beg_it != t_frames.top().end_it; ++t_frames.top().beg_it) {
                t_env->eval_quote(*t_frames.top().beg_it);
            }
        }
        else {
            for (auto arg_it = begin(t_frames.top().fnc.params); arg_it != end(t_frames.top().fnc.params); ++arg_it) {
                if (t_frames.top().beg_it == t_frames.top().end_it) {
                    t_frames.top().buf.emplace_back(*arg_it, t_env->t_symbols.nil);
                }
                else {
                    t_frames.top().buf.emplace_back(*arg_it, *t_frames.top().beg_it);
                    ++t_frames.top().beg_it;
                }
            }
        }
    }
    else if(is_spread(t_frames.top().fnc)){
        if (t_frames.top().fnc.params.empty()) throw "LambdaEvaler::eval_args: nospread without arg";
        DPair params_list;
        for (; t_frames.top().beg_it != t_frames.top().end_it; ++t_frames.top().beg_it) {
            params_list.emplace_back(*t_frames.top().beg_it);
        }
        t_frames.top().buf.emplace_back(t_frames.top().fnc.params[0], make_list(move(params_list)));
    }
    else {
        throw "LambdaEvaler::eval_args: unknown args_type";
    }
}
