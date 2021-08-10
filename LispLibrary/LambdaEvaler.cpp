#include "LambdaEvaler.h"
#include "CoreEnv.h"
#include "Symbol.h"
#include "Funcs.h"
using namespace std;

LambdaEvaler::LambdaEvaler(CoreEnvironment* env):
    ArgsOnStackEvaler(env)
{
}

Cell LambdaEvaler::pop_eval()
{
    eval_args();
    if (t_frames.top().buf.empty()) {
        //std::vector<Cell> body_buf = copy_dpair_list_to_vector(t_frames.top().fnc.body);
        CarCdrIteration iteration(t_frames.top().fnc.body, t_env->t_farm);
        Cell result = 
            env_eval_direct_bifunc(
                &CoreEnvironment::nbifunc_progn, begin(iteration), end(iteration), t_env
            );
        t_frames.pop();
        return result;
    }
    else {
        t_env->t_envs.push(move(t_frames.top().buf));
        //std::vector<Cell> body_buf = copy_dpair_list_to_vector(t_frames.top().fnc.body);
        CarCdrIteration iteration(t_frames.top().fnc.body, t_env->t_farm);
        Cell result = 
            env_eval_direct_bifunc(
                &CoreEnvironment::nbifunc_progn, begin(iteration), end(iteration), t_env
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
    if (is_spread(t_frames.top().fnc)) {
        if (!t_frames.top().forse_noeval_args) {
            CarCdrIteration iteration(t_frames.top().fnc.params, t_env->t_farm);
            for (auto arg_it = begin(iteration); arg_it != end(iteration); ++arg_it) {
                if (!is_symbol(*arg_it)) break;
                if (t_frames.top().beg_it == t_frames.top().end_it) {
                    t_frames.top().buf.emplace_back(to_symbol(*arg_it), t_env->t_farm.nil);
                }
                else {
                    t_frames.top().buf.emplace_back(to_symbol(*arg_it), t_env->eval_quote(*t_frames.top().beg_it));
                    ++t_frames.top().beg_it;
                }
            }
            for (; t_frames.top().beg_it != t_frames.top().end_it; ++t_frames.top().beg_it) {
                t_env->eval_quote(*t_frames.top().beg_it);
            }
        }
        else {
            CarCdrIteration iteration(t_frames.top().fnc.params, t_env->t_farm);
            for (auto arg_it = begin(iteration); arg_it != end(iteration); ++arg_it) {
                if (!is_symbol(*arg_it)) break;
                if (t_frames.top().beg_it == t_frames.top().end_it) {
                    t_frames.top().buf.emplace_back(to_symbol(*arg_it), t_env->t_farm.nil);
                }
                else {
                    t_frames.top().buf.emplace_back(to_symbol(*arg_it), *t_frames.top().beg_it);
                    ++t_frames.top().beg_it;
                }
            }
        }
    }
    else if(is_nospread(t_frames.top().fnc)){
        t_frames.top().buf.emplace_back(
                to_symbol(car(t_frames.top().fnc.params)),
                t_env->t_farm.make_list_cell(t_frames.top().beg_it, t_frames.top().end_it)
            );
    }
    else {
        throw "LambdaEvaler::eval_args: unknown args_type";
    }
}
