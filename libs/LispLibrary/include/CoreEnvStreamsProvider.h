#pragma once
#include <iostream>
#include "CoreStreamInt.h"


class CoreEnvStreamsProvider
{
public:
    CoreEnvStreamsProvider(std::unique_ptr<CoreInputStreamInt> &&input, std::unique_ptr<CoreOutputStreamInt>&& output);
    ~CoreEnvStreamsProvider() = default;

    CoreInputStreamInt& t_input_stream();
    CoreOutputStreamInt& t_output_stream();
    const CoreInputStreamInt& t_input_stream()const;
    const CoreOutputStreamInt& t_output_stream()const;
private:
    std::unique_ptr<CoreInputStreamInt> t_input;
    std::unique_ptr<CoreOutputStreamInt> t_output;
};

