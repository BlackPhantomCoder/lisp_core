#pragma once
#include "Types.h"
#include <iostream>


class CoreEnvStreamsProvider
{
public:
    CoreEnvStreamsProvider(std::istream& input, std::ostream& output, stream_read_mode mode = stream_read_mode::new_string);
    ~CoreEnvStreamsProvider() = default;

    void set_istream(std::istream& in);
    void set_ostream(std::ostream& out);

    void set_read_mode(stream_read_mode new_mode);
protected:
    stream_read_mode get_read_mode() const;
    std::istream& t_input_stream();
    std::ostream& t_output_stream();
    const std::istream& t_input_stream()const;
    const std::ostream& t_output_stream()const;
private:
    std::reference_wrapper<std::istream> t_input;
    std::reference_wrapper<std::ostream> t_output;
    stream_read_mode t_mode;
};

