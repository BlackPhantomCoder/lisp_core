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
    auto main_func = EvalQuote(*t_env, arg);
    t_frames.push(main_func);

	for(;;) {
        //mutex maybe here
        if ((t_env->t_stop_flag) && (*t_env->t_stop_flag).get().get()) throw CoreData::throw_stop_helper{};
		auto& frame = t_frames.top();
        auto& fnc = frame.get();

        switch (fnc.stage())
        {
        case stages::executed:
                {
                    auto buf = fnc.result();
                    t_frames.pop();
                    if (!empty(t_frames)) {
                        t_frames.top().get().push_next(buf);
                        t_frames.top().get().execute();
                    }
                    else {
                        return buf;
                    }
                }
            break;
        case stages::need_external:
                t_frames.push({ fnc.get_next() });
            break;
        case stages::intermediate:
        case stages::not_started:
                fnc.execute();
            break;
        default:
            throw "unknow stage";
            break;
        }
	}
}

void FuncsEvaler::clear()
{
	while (!t_frames.empty()) {
		t_frames.pop();
	}
}