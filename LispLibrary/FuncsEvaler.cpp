#include "FuncsEvaler.h"
#include "CoreEnv.h"
#include "SupportFuncs.h"
#include "Func.h"
using namespace std;

FuncsEvaler::FuncsEvaler(CoreEnvironment* env):
	t_env(env)
{
  
}

//Cell FuncsEvaler::eval(Cell& arg)
//{
//    return eval(make_fnc<EvalQuote>(*t_env, arg));
//}

Cell FuncsEvaler::eval(CoreData::HolderPtr&& func)
{
    t_frames.push_back(move(func));
    for (;;) {
        auto& frame = t_frames.back();
        if ((*frame.ptr).execute()) {
            if (t_frames.size() == 1) {
                auto buf = (*frame.ptr).result();
                t_frames.pop_back();
                return buf;
            }
            auto& prev_fnc = ((*(t_frames[t_frames.size() - 2]).ptr));
            prev_fnc.push_next((*frame.ptr).result());
            t_frames.pop_back();
        }
        else {
            if ((t_env->stop_flag()) && (*t_env->stop_flag()).get().get()) throw CoreData::throw_stop_helper{};
            t_frames.push_back((*frame.ptr).get_next());
        }
    }
}

void FuncsEvaler::clear()
{
	while (!t_frames.empty()) {
		t_frames.pop_back();
	}
    t_frames.shrink_to_fit();
}