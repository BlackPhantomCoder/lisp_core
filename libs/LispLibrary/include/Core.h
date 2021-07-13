#pragma once

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
    Core(std::istream& pred_funcs, std::istream& input, std::ostream& output);
    Core(std::istream& input, std::ostream& output);
    ~Core();

    CoreEnvStreamsProvider& streams();

    std::pair< result_type, Cell> execute_to_cell(const std::string& input);
    std::pair< result_type, std::string> execute(const std::string& input);

    std::pair< result_type, Cell> execute_to_cell(const Cell& input);
    std::pair< result_type, std::string> execute(const Cell& input);
    // Унаследовано через IPublicCoreEnvironmentProvider
    virtual const CoreEnvironment& get() const override;

    void stop();
private:
    Mutexted<bool> t_stop;
    CoreEnvironment t_env;
};

std::string to_string(Core::result_type rt);
std::ostream& operator<<(std::ostream& s, Core::result_type rt);

struct empty_streams {
    std::istringstream in;
    std::ostringstream out;
};

std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_empty_streams();
