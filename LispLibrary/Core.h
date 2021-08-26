#pragma once
#include "Funcs.h"
#include "CoreEnv.h"
#include "Mutexed.h"
#include <iostream>
#include <sstream>
#include <memory>

class Core
{
public:
    enum class result_type {
        success, fail, stopped
    };
public:
    Core(Core&& rh) noexcept;
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
private:
    Core();
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