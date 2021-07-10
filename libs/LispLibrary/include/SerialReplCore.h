#pragma once

#include "CoreEnv.h"
#include "Funcs.h"
#include <string>
#include <iostream>

class SerialReplCore {
public:
    enum class result_type {
        success, fail
    };
public:
    SerialReplCore() = default;
    ~SerialReplCore() = default;
    std::pair< result_type, std::string> execute(const std::string& input);
    std::pair< result_type, std::string> execute(const Cell& input);
private:
    CoreEnvironment env;
};

std::ostream& operator<<(std::ostream& s, SerialReplCore::result_type rt);