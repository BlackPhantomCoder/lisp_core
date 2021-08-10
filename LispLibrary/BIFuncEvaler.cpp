#include "BIFuncEvaler.h"
#include "CoreEnv.h"
#include "Funcs.h"

using namespace std;

BifuncEvaler::BifuncEvaler(CoreEnvironment* env) :
    ArgsOnStackEvaler(env)
{
}

Cell BifuncEvaler::pop_eval()
{
    {
        auto& last_frame = t_frames.top();
        if (last_frame.type == bifunc_type::bifunc && !last_frame.forse_nosread_args) {
            last_frame.args = t_env->t_farm.make_list_w_eval_cell(last_frame.beg_it, last_frame.end_it);
            CarCdrIteration iteration(last_frame.args, t_env->t_farm);
            t_env->t_args.push(begin(iteration), end(iteration));
        }
        else {
            t_env->t_args.push(last_frame.beg_it, last_frame.end_it);
        }
    }

    auto result = (t_env->*(t_frames.top().fnc))();
    t_env->t_args.pop();
    t_frames.pop();
    return result;
}