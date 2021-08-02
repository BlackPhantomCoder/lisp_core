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

Core::Core(CoreInputStreamInt& predfuncs, std::unique_ptr<CoreEnvStreamsProvider>&& streams, std::unique_ptr<IMutexed<bool>>&& custom_mutex):
    t_streams(move(streams)),
    t_env(*t_streams.get())
{
    t_stop = move(custom_mutex); 
    t_stop->set(false);
    this->execute_all(predfuncs);
}

Core::Core(std::unique_ptr<CoreEnvStreamsProvider>&& streams, std::unique_ptr<IMutexed<bool>>&& custom_mutex):
    t_streams(move(streams)),
    t_env(*t_streams.get())
{
    t_stop = move(custom_mutex);
    t_stop->set(false);
}

Core::Core(CoreInputStreamInt& predfuncs, std::unique_ptr<CoreEnvStreamsProvider>&& streams) :
    t_streams(move(streams)),
    t_env(*t_streams.get())
{
    t_stop = make_unique<STDMutexed<bool>>(false);
    this->execute_all(predfuncs);
}

Core::Core(std::unique_ptr<CoreEnvStreamsProvider>&& streams) :
    t_streams(move(streams)),
    t_env(*t_streams.get())
{
    t_stop = make_unique<STDMutexed<bool>>(false);
}

Core::~Core()
{
    if (t_stop) {
        t_stop->set(true);
    }
}

CoreEnvStreamsProvider& Core::streams()
{
    return *t_streams.get();
}

std::pair<Core::result_type, std::vector<std::string>> Core::execute_all(CoreInputStreamInt& input_s)
{
    if (t_stop->get()) {
        t_stop->set(false);
    }
    bool success = true;
    std::vector<std::string> result;
    try
    {
        std::vector<Cell> res_buf = t_env.execute_all(input_s, *t_stop);
        result.reserve(res_buf.size());
        for (const auto& c : res_buf) {
            result.emplace_back(to_string(c));
        }
    }
    catch (const char* str)
    {
        success = false;
        result = { str };
    }
    catch (const std::string& str)
    {
        success = false;
        result = { str };
    }
    catch (const throw_stop_helper&)
    {
        return { Core::result_type::stopped, {""} };
    }
    catch (...)
    {
        success = false;
        result = { "unknown error (wrong catch)" };
    }
    if (!success) return { Core::result_type::fail, std::move(result) };
    return{ Core::result_type::success, std::move(result) };
}

std::pair<Core::result_type, std::vector<std::string>> Core::execute_all(const std::string& input)
{
    stringstream s(input);
    StdCoreInputStream sc(s, stream_read_mode::new_string);
    return execute_all(sc);
}

std::pair<Core::result_type, std::string> Core::execute_one(CoreInputStreamInt& input_s)
{
    if (t_stop->get()) {
        t_stop->set(false);
    }
    bool success = true;
    std::string result;
    try
    {
        result = to_string(t_env.execute_one(input_s, *t_stop));
    }
    catch (const char* str)
    {
        success = false;
        result = str;
    }
    catch (const std::string& str)
    {
        success = false;
        result = { str };
    }
    catch (const throw_stop_helper&)
    {
        return { Core::result_type::stopped, "" };
    }
    catch (...)
    {
        success = false;
        result = "unknown error (wrong catch)";
    }
    if (!success) return { Core::result_type::fail, std::move(result) };
    return{ Core::result_type::success, std::move(result) };
}

std::pair<Core::result_type, std::string> Core::execute_one(const std::string& input)
{
    stringstream s(input);
    StdCoreInputStream sc(s, stream_read_mode::new_string);
    return execute_one(sc);
}

const CoreEnvironment& Core::get() const
{
    return t_env;
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

std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_empty_streams()
{
    auto s = std::make_unique<empty_streams>();
    return std::pair<Core, std::unique_ptr<empty_streams>>{
        std::piecewise_construct_t(),
        std::forward_as_tuple(
            make_unique<CoreEnvStreamsProvider>(
                make_unique<StdCoreInputStream>(s->in, stream_read_mode::new_string), make_unique<StdCoreOutputStream>(s->out)
            )
        ),
        std::forward_as_tuple(move(s)) 
    };
}

std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_empty_streams(std::unique_ptr<IMutexed<bool>>&& custom_mutex)
{
    auto s = std::make_unique<empty_streams>();
    return std::pair<Core, std::unique_ptr<empty_streams>>{
        std::piecewise_construct_t(),
        std::forward_as_tuple(
            make_unique<CoreEnvStreamsProvider>(
                make_unique<StdCoreInputStream>(s->in, stream_read_mode::new_string), make_unique<StdCoreOutputStream>(s->out)
            ),
            move(custom_mutex)
        ),
        std::forward_as_tuple(move(s)) 
    };
}
