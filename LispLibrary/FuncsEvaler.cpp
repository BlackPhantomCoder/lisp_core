#include "FuncsEvaler.h"
#include "Funcs.h"
#include "CoreEnv.h"
#include "BiFuncs.h"
using namespace std;

FuncsEvaler::FuncsEvaler(CoreEnvironment* env):
	t_env(env)
{
  
}

Cell FuncsEvaler::eval(Cell& arg)
{
    t_frames.push_back(make_fnc<EvalQuote>(*t_env, arg));

    for (;;) {
        auto& frame = t_frames.back();
        if (frame.func.execute()) {
            if (t_frames.size() == 1) {
                auto buf = frame.func.result();
                t_frames.pop_back();
                return buf;
            }
            auto& prev_fnc = (t_frames[t_frames.size() - 2].func);
            prev_fnc.push_next(frame.func.result());
            t_frames.pop_back();
        }
        else {
            if ((t_env->t_stop_flag) && (*t_env->t_stop_flag).get().get()) throw CoreData::throw_stop_helper{};
            t_frames.push_back(frame.func.get_next());
        }
    }
}

void FuncsEvaler::clear()
{
	while (!t_frames.empty()) {
		t_frames.pop_back();
	}
    t_frames.shrink_to_fit();
    //CoreData::funcs_pools_clear();
}