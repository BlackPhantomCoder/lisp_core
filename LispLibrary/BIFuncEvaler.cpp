#include "BIFuncEvaler.h"
#include "CoreEnv.h"

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
            last_frame.args.reserve(std::distance(last_frame.beg_it, last_frame.end_it));
            for (; last_frame.beg_it != last_frame.end_it; ++last_frame.beg_it) {
                last_frame.args.push_back(t_env->eval_quote(*last_frame.beg_it));
            }
            t_env->t_args.push(begin(last_frame.args), end(last_frame.args));
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