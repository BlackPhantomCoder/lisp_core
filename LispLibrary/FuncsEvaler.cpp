#include "FuncsEvaler.h"
#include "CoreEnv.h"
#include "SupportFuncs.h"
#include "Func.h"

#include "Bifuncs.h"
#include "SupportFuncs.h"
#include "FuncID.h"

using namespace std;

FuncsEvaler::FuncsEvaler(CoreEnvironment* env):
	t_env(env)
{
  
}

Cell FuncsEvaler::eval(CoreData::HolderPtr&& func)
{
    t_funcs.push_back(move(func));
    //cout << "-----------eval start ----------" << endl;
    for (;;) {
        auto* func = t_funcs.back().get();
        auto* data = &func_table[(unsigned char)func->id()];
        {
            for (auto calls = 0; calls < 100; ++calls) {
                switch (func->stage())
                {
                case stages::before_args_eval:
                    if (data->func_before_args) {
                        data->func_before_args(func, *t_env);
                        break;
                    }
                    else
                        func->f_next();
                    [[fallthrough]];
                case stages::args_eval:
                    switch (data->core_type)
                    {
                    case func_data::core_type::range_nbifunc:
                    case func_data::core_type::func:
                        break;
                    case  func_data::core_type::range_bifunc:
                        ((RangeBiFunc*)func)->t_eval_args();
                        break;
                    default:
                        throw logic_error("func_execute: unknown core_type");
                    }
                    break;
                case stages::after_args_eval:
                    if (data->func_after_args) {
                        data->func_after_args(func, *t_env);
                        break;
                    }
                    else
                        func->f_next();
                    [[fallthrough]];
                case stages::execution:
                    data->func_execute(func, *t_env);
                    break;
                case stages::executed:
                    {
                        //cout << "result: " << func->id() << endl;
                        if (t_funcs.size() == 1) {
                            auto buf = func->s_result();
                            t_funcs.pop_back();
                            //cout << "-----------eval end ----------" << endl;
                            return buf;
                        }
                        auto& prev_fnc = t_funcs[t_funcs.size() - 2];
                        prev_fnc->f_push_next(func->s_result());
                        t_funcs.pop_back();
                        func = t_funcs.back().get();
                        data = &func_table[(unsigned char)func->id()];
                    }
                    continue;
                case stages::need_external_before_args_eval:
                case stages::need_external_args_eval:
                case stages::need_external_after_args_eval:
                case stages::need_external_execution:
                case stages::need_external_return_next:
                case stages::need_external_before_args_eval_plus_next:
                case stages::need_external_args_eval_plus_next:
                case stages::need_external_after_args_eval_plus_next:
                    t_funcs.push_back(func->s_get_next());
                    func = t_funcs.back().get();
                    data = &func_table[(unsigned char)func->id()];
                    //cout << "external: " << t_funcs.back()->id() << endl;
                    continue;
                default:
                    throw logic_error("func_execute: unknown stage");
                }
                func->f_next();
            }
            if ((t_env->stop_flag()) && (*t_env->stop_flag()).get().get()) throw CoreData::throw_stop_helper{};
        }
    }
}

void FuncsEvaler::clear()
{
	while (!t_funcs.empty()) {
        t_funcs.pop_back();
	}
    t_funcs.shrink_to_fit();
}