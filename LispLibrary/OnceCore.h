#pragma once

#include "Mutexed.h"
#include "CoreEnv.h"
#include "Funcs.h"
#include <string>

class OnceCore {
public:
    enum class result_type {
        success, fail, forsed_stopped
    };
public:
    OnceCore(const std::string& input);
    ~OnceCore();

    void forse_stop();

    bool is_finished()const;
    std::pair< result_type, std::string> result()const;
private:
    Mutexted<std::string> t_result;
    Mutexted<result_type> t_result_type;
    Mutexted<bool> t_finished;

    std::future<void> t_handle;
};