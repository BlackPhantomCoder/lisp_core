#include "BIFuncEvaler.h"
#include "CoreEnv.h"

using namespace std;

BifuncEvaler::BifuncEvaler(CoreEnvironment* env) :
    t_env(env)
{
}

void BifuncEvaler::push_bifunc(CoreData::bifunc fnc, Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it, bool eval_args)
{
    t_frames.push({ fnc, beg_it , end_it ,eval_args });
}

void BifuncEvaler::push_nbifunc(CoreData::bifunc fnc, Cell::olist::const_iterator beg_it, Cell::olist::const_iterator end_it)
{
    t_frames.push({ fnc, beg_it , end_it ,false });
}

Cell BifuncEvaler::pop_eval()
{
    if (t_frames.top().eval_args) {
        t_frames.top().args = t_env->eval_args(t_frames.top().beg_it, t_frames.top().end_it);
        t_env->t_args.push(begin(t_frames.top().args), end(t_frames.top().args));
        auto result = (t_env->*(t_frames.top().fnc))();
        t_env->t_args.pop();
        t_frames.pop();
        return result;
    }
    else {
        t_env->t_args.push(t_frames.top().beg_it, t_frames.top().end_it);
        auto result = (t_env->*(t_frames.top().fnc))();
        t_env->t_args.pop();
        t_frames.pop();
        return result;
    }
}

void BifuncEvaler::clear()
{
    while (!t_frames.empty()) {
        t_frames.pop();
    }
}
