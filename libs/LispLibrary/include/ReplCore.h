#pragma once

#include "Mutexed.h"
#include "CoreEnv.h"
#include "Funcs.h"
#include <string>
#include <memory>

class ReplCore {
public:
    enum class result_type {
        success, fail, forsed_stopped
    };
public:
    ReplCore();
    ~ReplCore();

    // if called while running -> forse stop + new execute
    void execute(const std::string& input);
    void forse_stop();

    bool is_finished()const;
    std::pair< result_type, std::string> last_result()const;
private:
    CoreEnvironment env;
    Mutexted<std::string> t_result;
    Mutexted<result_type> t_result_type;
    Mutexted<bool> t_finished = true;

    std::future<void> t_handle;
};