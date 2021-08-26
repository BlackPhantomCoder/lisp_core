#include "CoreEnvStreamsProvider.h"

RefCESP::RefCESP(
    CoreInputStreamInt& input, CoreOutputStreamInt& output
):
    t_input(input),
    t_output(output)
{
}

CoreInputStreamInt& RefCESP::t_input_stream()
{
    return t_input;
}

CoreOutputStreamInt& RefCESP::t_output_stream()
{
    return t_output;
}

const CoreInputStreamInt& RefCESP::t_input_stream() const
{
    return t_input;
}

const CoreOutputStreamInt& RefCESP::t_output_stream() const
{
    return t_output;
}

ValCESP::ValCESP(
    std::unique_ptr<CoreInputStreamInt>&& input, std::unique_ptr<CoreOutputStreamInt>&& output
):
    t_input(move(input)),
    t_output(move(output))
{
}

CoreInputStreamInt& ValCESP::t_input_stream()
{
    return *t_input;
}

CoreOutputStreamInt& ValCESP::t_output_stream()
{
    return *t_output;
}

const CoreInputStreamInt& ValCESP::t_input_stream() const
{
    return *t_input;
}

const CoreOutputStreamInt& ValCESP::t_output_stream() const
{
    return *t_output;
}