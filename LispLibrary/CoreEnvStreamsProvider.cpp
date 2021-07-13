#include "CoreEnvStreamsProvider.h"

CoreEnvStreamsProvider::CoreEnvStreamsProvider(std::istream& input, std::ostream& output, stream_read_mode mode):
    t_input(input),
    t_output(output),
    t_mode(mode)
{
}

void CoreEnvStreamsProvider::set_istream(std::istream& in)
{
    t_input = { in };
}

void CoreEnvStreamsProvider::set_ostream(std::ostream& out)
{
    t_output = { out };
}

void CoreEnvStreamsProvider::set_read_mode(stream_read_mode new_mode)
{
    t_mode = new_mode;
}

stream_read_mode CoreEnvStreamsProvider::get_read_mode() const
{
    return t_mode;
}

std::istream& CoreEnvStreamsProvider::t_input_stream()
{
    return t_input.get();
}

std::ostream& CoreEnvStreamsProvider::t_output_stream()
{
    return t_output.get();
}

const std::istream& CoreEnvStreamsProvider::t_input_stream() const
{
    return t_input.get();
}

const std::ostream& CoreEnvStreamsProvider::t_output_stream() const
{
    return t_output.get();
}
