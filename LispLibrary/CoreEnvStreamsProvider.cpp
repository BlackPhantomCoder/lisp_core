#include "CoreEnvStreamsProvider.h"

CoreEnvStreamsProvider::CoreEnvStreamsProvider(
    std::unique_ptr<CoreInputStreamInt>&& input, std::unique_ptr<CoreOutputStreamInt>&& output
):
    t_input(move(input)),
    t_output(move(output))
{
}

CoreInputStreamInt& CoreEnvStreamsProvider::t_input_stream()
{
    return *t_input.get();
}

CoreOutputStreamInt& CoreEnvStreamsProvider::t_output_stream()
{
    return *t_output.get();
}

const CoreInputStreamInt& CoreEnvStreamsProvider::t_input_stream() const
{
    return *t_input.get();
}

const CoreOutputStreamInt& CoreEnvStreamsProvider::t_output_stream() const
{
    return *t_output.get();
}
