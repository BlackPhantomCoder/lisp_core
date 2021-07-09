#include "OnceCore.h"

OnceCore::OnceCore(const std::string& input):
    t_finished(false)
{
    t_handle = std::async(std::launch::async, [this, &input]() {

        CoreEnvironment env;
        bool success = true;
        std::string result;

        try
        {
            result = to_string(env.execute_one(read(input), t_finished));
        }
        catch (...)
        {
            success = false;
            result = "error";
        }
        t_result.set(std::move(result));
        t_result_type.set((success) ? result_type::success : result_type::fail);
        t_finished.set(true);
        });
}

OnceCore::~OnceCore()
{
    t_handle.get();
}

void OnceCore::forse_stop()
{
    t_result_type.set(result_type::forsed_stopped);
    t_finished.set(true);
}

bool OnceCore::is_finished() const
{
    return t_finished.get();
}

std::pair<OnceCore::result_type, std::string> OnceCore::result() const
{
    return { t_result_type.get(), t_result.get() };
}