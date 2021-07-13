#include "Core.h"
#include "Funcs.h"

#include <tuple>
#include  <utility>

using namespace std;

Core::Core(std::istream& pred_funcs, std::istream& input, std::ostream& output) :
    t_stop(false),
    t_env(input, output)
{
    for (const auto& exp : read_expressions_from_stream(pred_funcs, *this, stream_read_mode::s_expression, true)) {
        Core::result_type result_reason = Core::result_type::success;
        this->execute(exp);
    }
}

Core::Core(std::istream& input, std::ostream& output) :
    t_stop(false),
    t_env(input, output)
{
}

Core::~Core()
{
    t_stop.set(true);
}

CoreEnvStreamsProvider& Core::streams()
{
    return t_env;
}

std::pair<Core::result_type, Cell> Core::execute_to_cell(const std::string& input)
{
    auto [result_reason, cell] = read_one_expression_from_string(input, *this, false);
    if (!result_reason) return { Core::result_type::fail,   make_atom(Atom("wrong input" ))};
    return execute_to_cell(cell);
}

std::pair<Core::result_type, std::string> Core::execute(const std::string& input)
{
    auto [result_reason, cell] = read_one_expression_from_string(input, *this, false);
    if (!result_reason) return { Core::result_type::fail,   "wrong input" };
    return execute(cell);
}

std::pair<Core::result_type, Cell> Core::execute_to_cell(const Cell& input)
{
    bool success = true;
    Cell result;
    try
    {
        result = t_env.execute_one(input, t_stop);
    }
    catch (const char* str)
    {
        success = false;
        result = make_atom(Atom(str));
    }
    catch (...)
    {
        success = false;
        result = make_atom(Atom("unknown error (wrong catch)"));
    }
    if (!success && !t_stop.get()) return { Core::result_type::fail, move(result) };
    if(!success && t_stop.get())return { Core::result_type::stopped, move(result) };
    return{ Core::result_type::success, move(result) };
}

std::pair<Core::result_type, std::string> Core::execute(const Cell& input)
{
    bool success = true;
    std::string result;
    try
    {
        result = to_string(t_env.execute_one(input, t_stop));
    }
    catch (const char* str)
    {
        success = false;
        result = str;
    }
    catch (...)
    {
        success = false;
        result = "unknown error (wrong catch)";
    }
    if (!success && !t_stop.get()) return { Core::result_type::fail, move(result) };
    if (!success && t_stop.get())return { Core::result_type::stopped, move(result) };
    return{ Core::result_type::success, move(result) };
}

const CoreEnvironment& Core::get() const
{
    return t_env;
}

void Core::stop()
{
    t_stop.set(false);
}

std::string to_string(Core::result_type rt) {
    switch (rt)
    {
    case Core::result_type::success:
        return "success";
        break;
    case Core::result_type::fail:
        return "fail";
        break;
    case Core::result_type::stopped:
        return "stopped";
        break;
    }
    throw "unknown Core::result_type";
}

std::ostream& operator<<(std::ostream& s, Core::result_type rt) {
    s << to_string(rt);
    return s;
}

std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_empty_streams()
{
    auto s = std::make_unique<empty_streams>();
    return std::pair<Core, std::unique_ptr<empty_streams>>{ std::piecewise_construct_t(), std::forward_as_tuple(s->in, s->out), std::forward_as_tuple(move(s)) };
}
