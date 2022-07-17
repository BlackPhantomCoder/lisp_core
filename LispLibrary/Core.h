#pragma once
#include "Funcs.h"
#include "CoreEnv.h"
#include "Mutexed.h"
#include "Errors.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <optional>

class Core
{
    friend Core make_core();
public:
    enum class result_type {
        success, fail, stopped
    };
public:
    Core(Core&& rh) noexcept;
    Core(std::istream& state_stream, std::unique_ptr<IMutexed<bool>>&& custom_mutex);
    Core(std::istream& state_stream);

    Core(
        CoreEnvStreamsProviderInt& predfuncs,
        std::unique_ptr<IMutexed<bool>>&& custom_mutex
    );
    Core(std::unique_ptr<IMutexed<bool>>&& custom_mutex);

    Core(CoreEnvStreamsProviderInt& predfuncs);
    ~Core();

    result_type execute_all(CoreEnvStreamsProviderInt& streams);
    result_type execute_one(CoreEnvStreamsProviderInt& streams);
    result_type execute_driver(CoreEnvStreamsProviderInt& streams);

    void stop();


    // сохрание состояния окружения в поток
    // (не полного)
    void save_state(std::ostream& os);

    // загрузка из состояния
    // (очищает текущее)
    bool load_state(std::istream& is);
private:
    Core(
        std::optional<std::reference_wrapper<std::istream>> state_stream,
        std::optional<std::unique_ptr<IMutexed<bool>>> custom_mutex
    );
    template <class Fnc>
    result_type t_under_catch(Fnc fnc, CoreEnvStreamsProviderInt& streams, bool eos_error = true);
private:
    std::unique_ptr<IMutexed<bool>> t_stop;
    std::unique_ptr <CoreEnvironment> t_env;
};

std::string to_string(Core::result_type rt);
std::ostream& operator<<(std::ostream& s, Core::result_type rt);

struct empty_streams {
    std::istringstream in;
    std::ostringstream out;
};










template <class Fnc>
Core::result_type Core::t_under_catch(Fnc fnc, CoreEnvStreamsProviderInt& streams, bool eos_error) {
    auto result = Core::result_type::fail;
#ifndef EX_CATCH
    try
    {
        fnc();
        result = Core::result_type::success;
    }
    catch (errors::eos& e)
    {
        if (!eos_error)
            result = Core::result_type::success;
        else throw e;
    }
#endif

#ifdef EX_CATCH
    try
    {
        fnc();
        result = Core::result_type::success;
    }
    catch (errors::eos&)
    {
        if(eos_error)
            streams.t_output_stream().out_new_line("error: eos");
        else
            result = Core::result_type::success;
    }
    catch (const char* str)
    {
        streams.t_output_stream().out_new_line(std::string("error: ") + str);
    }
    catch (std::string& str)
    {
        streams.t_output_stream().out_new_line(std::string("error: ") + str);
    }
    catch (std::exception& e)
    {
        streams.t_output_stream().out_new_line(std::string("error: ")  + e.what());
    }
    catch (errors::lexical_error&)
    {
        streams.t_output_stream().out_new_line("error: lexical_error");
    }
    catch (errors::syntax_error&)
    {
        streams.t_output_stream().out_new_line("error: syntax_error");
    }
    catch (errors::my_error&)
    {
        streams.t_output_stream().out_new_line("error: my_error");
    }
    catch (...)
    {
        streams.t_output_stream().out_new_line("error: unknown");
    }
#endif
    return result;
}