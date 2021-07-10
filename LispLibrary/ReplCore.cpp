#include "ReplCore.h"

ReplCore::ReplCore():
    t_finished(true),
    t_result_type(ReplCore::result_type::success),
    t_result("")
{
}

ReplCore::~ReplCore()
{
    t_handle.get();
}

void ReplCore::execute(const std::string& input)
{
    if (!t_finished.get()) {
        forse_stop();
        t_handle.get();
    }
    t_handle = std::future<void>();
    t_finished.set(false);
    t_handle = std::async(std::launch::async, [this, &input]() {
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
        t_result_type.set((success) ? ReplCore::result_type::success : ReplCore::result_type::fail);
        t_finished.set(true);
    });
}

void ReplCore::forse_stop()
{
    t_result_type.set(result_type::forsed_stopped);
    t_finished.set(true);
}

bool ReplCore::is_finished() const
{
    return t_finished.get();
}

std::pair<ReplCore::result_type, std::string> ReplCore::last_result() const
{
    return { t_result_type.get(), t_result.get() };
}