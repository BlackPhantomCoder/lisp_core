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
        std::vector<Cell> body_buf = copy_dpair_list_to_vector(t_frames.top().fnc.body);
        Cell result = 
            env_eval_direct_bifunc(
                &CoreEnvironment::nbifunc_progn, begin(body_buf), end(body_buf), t_env
            );
        t_frames.pop();
        return result;
    }
    else {
        t_env->t_envs.push(move(t_frames.top().buf));
        std::vector<Cell> body_buf = copy_dpair_list_to_vector(t_frames.top().fnc.body);
        Cell result = 
            env_eval_direct_bifunc(
                &CoreEnvironment::nbifunc_progn, begin(body_buf), end(body_buf), t_env
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
    else if(is_nospread(t_frames.top().fnc)){
        if (t_frames.top().fnc.params.empty()) throw "LambdaEvaler::eval_args: nospread without arg";
        std::vector<Cell> params_list;
        params_list.reserve(std::distance(t_frames.top().beg_it, t_frames.top().end_it));
        for (; t_frames.top().beg_it != t_frames.top().end_it; ++t_frames.top().beg_it) {
            params_list.push_back(*t_frames.top().beg_it);
        }
        t_frames.top().buf.emplace_back(
                t_frames.top().fnc.params[0], make_list_cell(begin(params_list), end(params_list), t_env->t_symbols)
            );
    }
    else {
        throw "LambdaEvaler::eval_args: unknown args_type";
    }
}
