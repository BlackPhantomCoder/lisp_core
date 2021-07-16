#pragma once
#include <exception>

enum class stream_read_mode { new_string, s_expression };

struct throw_stop_helper :std::exception {

};
