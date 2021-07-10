#pragma once

#include "CoreEnv.h"
#include "Funcs.h"
#include <string>

class SerialReplCore {
public:
    enum class result_type {
        success, fail
    };
public:
    SerialReplCore() = default;
    ~SerialReplCore() = default;
    std::pair< result_type, std::string> execute(const std::string& input);
private:
    CoreEnvironment env;
};