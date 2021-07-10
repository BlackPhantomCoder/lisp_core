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
    catch (...)
    {
        success = false;
        result = "error";
    }
    return { ((success) ? SerialReplCore::result_type::success : SerialReplCore::result_type::fail) , move(result) };
}
