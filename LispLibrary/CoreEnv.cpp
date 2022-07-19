#include "CoreEnv.h"
#include "Funcs.h"
#include "ArgsCounter.h"
#include "profile.h"


#include "SupportFuncs.h"
#include "Bifuncs.h"
#include "FuncID.h"
#include "json/include/json.hpp"


#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;
using namespace CoreData;
using namespace nlohmann;

#define arg1 *b
#define arg2 *(next(b, 1))
#define arg3 *(next(b, 2))


CoreEnvironment::CoreEnvironment(std::optional<std::reference_wrapper<std::istream>> state) :
    t_farm(*this),
    t_funcs(*this),
    t_input(*this),
    t_scanner(*this),
    t_envs(*this),
    t_corefuncs(*this),
    t_macrotable(*this),
    t_cserial(*this),
    t_streams(nullopt),
    t_output_control(t_farm)
{
    if (state) {
        auto j = json();
        *state >> j;
        t_farm.init(j.at(0));
        t_funcs.init(j.at(1));
        t_envs.init(j.at(2));
        t_macrotable.init(j.at(3));
    }
    else {
        t_farm.init(nullopt);
        t_funcs.init(nullopt);
        t_envs.init(nullopt);
        t_macrotable.init(nullopt);
    }
}

CoreEnvironment::CoreEnvironment(
    CoreEnvStreamsProviderInt& streams,
    std::optional<std::reference_wrapper<std::istream>> state
) :
    CoreEnvironment(state)
{
    t_streams = streams;
}

void CoreEnvironment::t_clear()
{
    t_envs.clear_subenvs();

    t_clear_mem();
}

SExprsFarm& CoreEnvironment::farm()
{
    return t_farm;
}

std::optional<std::reference_wrapper<const IMutexed<bool>>>& CoreEnvironment::stop_flag()
{
    return t_stop_flag;
}

std::optional<std::reference_wrapper<CoreOutputStreamInt>>& CoreEnvironment::cos()
{
    return t_cos;
}

CoreFuncs& CoreEnvironment::core_funcs()
{
    return t_corefuncs;
}

OutputController& CoreEnvironment::output_control()
{
    return t_output_control;
}

CellEnvironment& CoreEnvironment::envs()
{
    return t_envs;
}

MacroTable& CoreEnvironment::macrotable()
{
    return t_macrotable;
}

Input& CoreEnvironment::input()
{
    return t_input;
}

Scanner& CoreEnvironment::scanner()
{
    return t_scanner;
}

FuncsStorage& CoreEnvironment::funcs()
{
    return t_funcs;
}

CellSerializer& CoreEnvironment::cserial()
{
    return t_cserial;
}

void CoreEnvironment::save_state(std::ostream& os)
{
    auto j = json();
    t_farm.save_state(j.emplace_back());
    t_funcs.save_state(j.emplace_back());
    t_envs.save_state(j.emplace_back());
    t_macrotable.save_state(j.emplace_back());
    os << j;
}

bool CoreEnvironment::load_state(std::istream& is)
{
    
    auto j = json();
    try
    {
        is >> j;
    }
    catch (const std::exception& e)
    {
        cout << e.what() << endl;
        return false;
    }
    catch (...)
    {
        return false;
    }


    try
    {
        t_farm.load_state(j.at(0));
        t_funcs.load_state(j.at(1));
        t_envs.load_state(j.at(2));
        t_macrotable.load_state(j.at(3));
    }
    catch (const std::exception& e)
    {
        cout << e.what() << endl;
        throw;
    }
    catch (...)
    {
        throw;
        //return false;
    }
    return true;
}

void CoreEnvironment::execute_all(CoreEnvStreamsProviderInt& streams, const IMutexed<bool>& stop_flag)
{
    return t_core_env_under_catch(
        [this, &streams, &stop_flag]() -> void {
            t_prepare(streams, stop_flag);
            std::vector<std::string> result;
            while (t_input.alive()) {
                t_execute();
            }
        }
    );
}

void CoreEnvironment::execute_one(CoreEnvStreamsProviderInt& streams, const IMutexed<bool>& stop_flag)
{
    return t_core_env_under_catch(
        [this, &streams, &stop_flag]() -> void {
            t_prepare(streams, stop_flag);
            t_execute();
        }
    );
}

void CoreEnvironment::execute_driver(CoreEnvStreamsProviderInt& streams, const IMutexed<bool>& stop_flag)
{
    t_prepare(streams, stop_flag);
    while(t_input.alive())
    {
        t_core_env_under_catch(
            [this, &streams, &stop_flag]() -> void {
                t_cos->get().out_without_new_line("> ");
                auto c = t_execute(make_fnc<Read>(CarCdrIterator(), CarCdrIterator()));
                auto a = DurationFunc([this](std::chrono::milliseconds time) {
                    t_cos->get().out_new_line(": " + to_string(time.count()) + " ms");
                    });
                if (!(!t_input.alive() && is_symbol(c) && t_output_control.to_string_raw(c) == "")) {
                    t_cos->get().out_new_line(t_output_control.to_string(t_execute(c)));
                }
            }
        );
    }
}

void CoreEnvironment::set_streams(CoreEnvStreamsProviderInt& streams)
{
    t_streams = streams;
}

void CoreEnvironment::t_prepare(CoreEnvStreamsProviderInt& streams, const IMutexed<bool>& stop_flag)
{
    t_streams = streams;
    t_stop_flag = { stop_flag };
    t_input.change_cis(t_streams->get().t_input_stream());
    t_cos = t_streams->get().t_output_stream();
}

void CoreEnvironment::t_execute()
{
    auto c = t_execute(make_fnc<Read>(CarCdrIterator(), CarCdrIterator()));
    t_cos->get().out_new_line(t_output_control.to_string(t_execute(c)));
}

Cell CoreEnvironment::t_execute(Cell& arg)
{
    return t_execute(make_fnc<EvalQuote>(arg));
}

Cell CoreEnvironment::t_execute(CoreData::HolderPtr&& f)
{
    auto funcs = vector<CoreData::HolderPtr, CoreData::allocator<CoreData::HolderPtr>>();
    funcs.push_back(move(f));
    //cout << "-----------eval start ----------" << endl;
    auto* func = funcs.back().get();
    auto* data = &func_table[(unsigned char)func->id()];
    for (;;) {
        {
            for (auto calls = 0; calls < 100; ++calls) {
                switch (func->stage())
                {
                case stages::before_args_eval:
                    if (data->func_before_args) {
                        data->func_before_args(func, *this);
                        func->f_next();
                        break;
                    }
                    else
                        func->f_next();
                    [[fallthrough]];
                case stages::args_eval:
                    //другие не предусмотрены
                    ((RangeBiFunc*)func)->t_eval_args();
                    func->f_next();
                    break;
                case stages::after_args_eval:
                    if (data->func_after_args) {
                        data->func_after_args(func, *this);
                        func->f_next();
                        break;
                    }
                    else
                        func->f_next();
                    [[fallthrough]];
                case stages::execution:
                    data->func_execute(func, *this);
                    func->f_next();
                    break;
                case stages::executed:
                {
                    //cout << "result: " << func->id() << endl;
                    if (funcs.size() == 1) {
                        auto buf = func->s_result();
                        funcs.pop_back();
                        //cout << "-----------eval end ----------" << endl;
                        return buf;
                    }
                    auto result = func->s_result();
                    funcs.pop_back();
                    func = funcs.back().get();
                    while(func->stage() == stages::need_external_return_next){
                        funcs.pop_back();
                        if (empty(funcs)) return result;
                        func = funcs.back().get();
                    }
                    func->f_push_next(result);
                    data = &func_table[(unsigned char)func->id()];
                }
                break;
                case stages::need_external_before_args_eval:
                case stages::need_external_args_eval:
                case stages::need_external_after_args_eval:
                case stages::need_external_execution:
                case stages::need_external_before_args_eval_plus_next:
                case stages::need_external_args_eval_plus_next:
                case stages::need_external_after_args_eval_plus_next:
                case stages::need_external_return_next:
                    funcs.push_back(func->s_get_next());
                    func = funcs.back().get();
                    data = &func_table[(unsigned char)func->id()];
                    //cout << "external: " << funcs.back()->id() << endl;
                    break;
                default:
                    throw logic_error("func_execute: unknown stage");
                }
            }
            if (stop_flag() && stop_flag()->get().get()) throw CoreData::throw_stop_helper{};
        }
    }
}

void CoreEnvironment::t_clear_mem()
{
    CoreData::allocator_release_memory<std::pair<Symbol, Cell>>();
    CoreData::allocator_release_memory<std::pair<size_t, size_t>>();
    CoreData::allocator_release_memory<Cell>();
    CoreData::allocator_release_memory<const Cell*>();
    CoreData::allocator_release_memory<CoreData::HolderPtr>();
    CoreData::funcs_pools_clear();
}
