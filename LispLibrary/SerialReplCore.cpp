#include "SerialReplCore.h"
#include "Mutexed.h"

std::pair<SerialReplCore::result_type, std::string> SerialReplCore::execute(const std::string& input)
{
    bool success = true;
    std::string result;
    try
    {
        result = to_string(env.execute_one(read(input),  Mutexted<bool>(false)));
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
    return { ((success) ? SerialReplCore::result_type::success : SerialReplCore::result_type::fail) , move(result) };
}

std::pair<SerialReplCore::result_type, std::string> SerialReplCore::execute(const Cell& input)
{
    bool success = true;
    std::string result;
    try
    {
        result = to_string(env.execute_one(input, Mutexted<bool>(false)));
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
    return { ((success) ? SerialReplCore::result_type::success : SerialReplCore::result_type::fail) , move(result) };
}

std::ostream& operator<<(std::ostream& s, SerialReplCore::result_type rt)
{
    switch (rt)
    {
    case SerialReplCore::result_type::success:
        s << "success";
        break;
    case SerialReplCore::result_type::fail:
        s << "fail";
        break;
    }
    return s;
}
