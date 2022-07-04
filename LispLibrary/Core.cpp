#include "Core.h"
#include "Funcs.h"
#include "STDMutexed.h"
#include "StdCoreInputStream.h"
#include "StdCoreOutputStream.h"
#include "Errors.h"

#include <tuple>
#include <utility>

using namespace std;
using namespace CoreData;

Core::Core(Core&& rh) noexcept:
    t_env(move(rh.t_env)),
    t_stop(move(rh.t_stop))
{

}

Core::Core(std::istream& state_stream, std::unique_ptr<IMutexed<bool>>&& custom_mutex):
    Core(std::optional<std::reference_wrapper<std::istream>>{state_stream}, move(custom_mutex))
{

}

Core::Core(std::istream& state_stream) :
    Core(std::optional<std::reference_wrapper<std::istream>>{ state_stream }, nullopt)
{
}

Core::Core(
    CoreEnvStreamsProviderInt& predfuncs,
    std::unique_ptr<IMutexed<bool>>&& custom_mutex
):
    Core(nullopt, move(custom_mutex))
{
    this->execute_all(predfuncs);
}

Core::Core(std::unique_ptr<IMutexed<bool>>&& custom_mutex) :
    Core(nullopt, move(custom_mutex))
{

}

Core::Core(CoreEnvStreamsProviderInt& predfuncs):
    Core(nullopt, nullopt)
{
    this->execute_all(predfuncs);
}

void Core::save_state(std::ostream& os)
{
    t_env->save_state(os);
}

bool Core::load_state(std::istream& is)
{
    return t_env->load_state(is);
}

Core::Core(
    std::optional<std::reference_wrapper<std::istream>> state_stream,
    std::optional<std::unique_ptr<IMutexed<bool>>> custom_mutex
):
    t_env(make_unique<CoreEnvironment>(state_stream))
{
        if (!custom_mutex)
            t_stop = make_unique<STDMutexed<bool>>(false);
        else {
            t_stop = std::move(*custom_mutex);
            t_stop->set(false);
        }
}


Core::~Core()
{
    t_env->t_clear();
    if (t_stop) {
        t_stop->set(true);
    }
}

Core::result_type Core::execute_all(CoreEnvStreamsProviderInt& streams)
{
    if (t_stop->get()) {
        t_stop->set(false);
    }
    return t_under_catch([this, &streams]() {t_env->execute_all(streams, *t_stop); }, streams, false);
}

Core::result_type Core::execute_one(CoreEnvStreamsProviderInt& streams)
{
    if (t_stop->get()) {
        t_stop->set(false);
    }
    return t_under_catch([this, &streams]() {t_env->execute_one(streams, *t_stop); }, streams );
}

Core::result_type Core::execute_driver(CoreEnvStreamsProviderInt& streams)
{
    if (t_stop->get()) {
        t_stop->set(false);
    }
    return t_under_catch([this, &streams]() {t_env->execute_driver(streams, *t_stop); }, streams, false);
}

void Core::stop()
{
    t_stop->set(true);
}

std::string to_string(Core::result_type rt) {
    switch (rt)
    {
    case Core::result_type::success:
        return "success";
        break;
    case Core::result_type::fail:
        return "fail";
        break;
    case Core::result_type::stopped:
        return "stopped";
        break;
    }
    throw "unknown Core::result_type";
}

std::ostream& operator<<(std::ostream& s, Core::result_type rt) {
    s << to_string(rt);
    return s;
}