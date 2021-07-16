#pragma once

#include "Types.h"
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
    Core(std::istream& pred_funcs, std::istream& input, std::ostream& output, std::unique_ptr<IMutexed<bool>>&& custom_mutex);
    Core(std::istream& input, std::ostream& output, std::unique_ptr<IMutexed<bool>>&& custom_mutex);

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
    template<class T>
    std::pair< result_type, Cell> execute_to_cell_temp(const T& input);
private:
    std::unique_ptr<IMutexed<bool>> t_stop;
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













template<class T>
inline std::pair<Core::result_type, Cell> Core::execute_to_cell_temp(const T& input)
{
    if (t_stop->get()) {
        t_stop->set(false);
    }
    bool success = true;
    Cell result;
    try
    {
        result = t_env.execute_one(input, *t_stop);
    }
    catch (const char* str)
    {
        success = false;
        result = make_atom(Atom(str));
    }
    catch (const throw_stop_helper&)
    {
        return { Core::result_type::stopped, make_atom(Atom("")) };
    }
    catch (...)
    {
        success = false;
        result = make_atom(Atom("unknown error (wrong catch)"));
    }
    if (!success) return { Core::result_type::fail, std::move(result) };
    return{ Core::result_type::success, std::move(result) };
}
