#pragma once
#include "Funcs.h"
#include "CoreEnv.h"
#include "PublicCoreEnvironmentProvider.h"
#include "Mutexed.h"
#include <iostream>
#include <sstream>
#include <memory>

class Core : public IPublicCoreEnvironmentProvider
{
public:
    enum class result_type {
        success, fail, stopped
    };
public:
    Core(Core&& rh) noexcept;
    Core(CoreInputStreamInt& predfuncs, std::unique_ptr<CoreEnvStreamsProvider>&& streams, std::unique_ptr<IMutexed<bool>>&& custom_mutex);
    Core(std::unique_ptr<CoreEnvStreamsProvider>&& streams, std::unique_ptr<IMutexed<bool>>&& custom_mutex);

    Core(CoreInputStreamInt& predfuncs, std::unique_ptr<CoreEnvStreamsProvider>&& streams);
    Core(std::unique_ptr<CoreEnvStreamsProvider>&& streams);
    ~Core();

    CoreEnvStreamsProvider& streams();
    std::pair< result_type, std::vector<std::string>> execute_all(CoreInputStreamInt& input_s);
    std::pair< result_type, std::vector<std::string>> execute_all(const std::string& input);

    std::pair< result_type, std::string> execute_one(CoreInputStreamInt& input_s);
    std::pair< result_type, std::string> execute_one(const std::string& input);
    // Унаследовано через IPublicCoreEnvironmentProvider
    virtual const CoreEnvironment& get() const override;

    void stop();
private:
    std::unique_ptr<IMutexed<bool>> t_stop;
    std::unique_ptr<CoreEnvStreamsProvider> t_streams;
    CoreEnvironment t_env;
    
};

std::string to_string(Core::result_type rt);
std::ostream& operator<<(std::ostream& s, Core::result_type rt);

struct empty_streams {
    std::istringstream in;
    std::ostringstream out;
};

std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_empty_streams();
std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_empty_streams(std::unique_ptr<IMutexed<bool>>&& custom_mutex);