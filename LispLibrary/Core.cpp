#include "Core.h"
#include "Funcs.h"
#include "STDMutexed.h"
#include "StdCoreInputStream.h"
#include "StdCoreOutputStream.h"

#include <tuple>
#include <utility>


using namespace std;
using namespace CoreData;

Core::Core(Core&& rh) noexcept:
    t_env(move(rh.t_env)),
    t_stop(move(rh.t_stop))
{

}

Core::Core(
    CoreEnvStreamsProviderInt& predfuncs,
    std::unique_ptr<IMutexed<bool>>&& custom_mutex
):
    Core(move(custom_mutex))
{
    this->execute_all(predfuncs);
}

Core::Core(std::unique_ptr<IMutexed<bool>>&& custom_mutex) :
    Core()
{
    t_stop = move(custom_mutex);
    t_stop->set(false);
}

Core::Core(CoreEnvStreamsProviderInt& predfuncs):
    Core()
{
    this->execute_all(predfuncs);
}

Core::Core() :
    t_env(make_unique<CoreEnvironment>())
{
    t_stop = make_unique<STDMutexed<bool>>(false);
}

Core::~Core()
{
    if (t_stop) {
        t_stop->set(true);
    }
}

Core::result_type Core::execute_all(CoreEnvStreamsProviderInt& streams)
{
    if (t_stop->get()) {
        t_stop->set(false);
    }
    bool success = true;

    #ifndef EX_CATCH
    t_env->execute_all(streams, *t_stop);;
    #endif

    #ifdef EX_CATCH
        try
        {
           t_env->execute_all(streams, *t_stop);
        }
        catch (const char* str)
        {
            success = false;
            streams.t_output_stream().out_new_line("error: "s + str);
        }
        catch (const std::string& str)
        {
            success = false;
            streams.t_output_stream().out_new_line("error: "s + str);
        }
        catch (const throw_stop_helper&)
        {
            return Core::result_type::stopped;
        }
        catch (...)
        {
            success = false;
            streams.t_output_stream().out_new_line("error: "s + "unknown");
        }
    #endif
    if (!success) return Core::result_type::fail;
    return Core::result_type::success;
}

Core::result_type Core::execute_one(CoreEnvStreamsProviderInt& streams)
{
    if (t_stop->get()) {
        t_stop->set(false);
    }
    bool success = true;

    #ifndef EX_CATCH
        t_env->execute_one(streams, *t_stop);
    #endif

    #ifdef EX_CATCH
        try
        {
            t_env->execute_one(streams, *t_stop);
        }
        catch (const char* str)
        {
            success = false;
            streams.t_output_stream().out_new_line("error: "s + str);
        }
        catch (const std::string& str)
        {
            success = false;
            streams.t_output_stream().out_new_line("error: "s + str);
        }
        catch (const throw_stop_helper&)
        {
            return Core::result_type::stopped;
        }
        catch (...)
        {
            success = false;
            streams.t_output_stream().out_new_line("error: "s + "unknown");
        }
    #endif
        if (!success) return Core::result_type::fail;
        return Core::result_type::success;
}

Core::result_type Core::execute_driver(CoreEnvStreamsProviderInt& streams)
{
    if (t_stop->get()) {
        t_stop->set(false);
    }
    bool success = true;

    #ifndef EX_CATCH
        t_env->execute_driver(streams, *t_stop);
    #endif
        
    #ifdef EX_CATCH
        try
        {
            t_env->execute_driver(streams, *t_stop);
        }
        catch (const char* str)
        {
            success = false;
            streams.t_output_stream().out_new_line("error: "s + str);
        }
        catch (const std::string& str)
        {
            success = false;
            streams.t_output_stream().out_new_line("error: "s + str);
        }
        catch (const throw_stop_helper&)
        {
            return Core::result_type::stopped;
        }
        catch (...)
        {
            success = false;
            streams.t_output_stream().out_new_line("error: "s + "unknown");
        }
    #endif
    if (!success) return Core::result_type::fail;
    return Core::result_type::success;
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