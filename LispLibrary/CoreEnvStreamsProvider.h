#pragma once
#include <iostream>
#include "CoreStreamInt.h"
#include "StdCoreInputStream.h"
#include "StdCoreOutputStream.h"

class CoreEnvStreamsProviderInt
{
public:
    virtual ~CoreEnvStreamsProviderInt() = default;

    virtual CoreInputStreamInt& t_input_stream() = 0;
    virtual CoreOutputStreamInt& t_output_stream() = 0;
    virtual const CoreInputStreamInt& t_input_stream()const = 0;
    virtual const CoreOutputStreamInt& t_output_stream()const = 0;
};

class RefCESP : public CoreEnvStreamsProviderInt
{
public:
    RefCESP(CoreInputStreamInt& input, CoreOutputStreamInt& output);
    ~RefCESP() = default;

    virtual CoreInputStreamInt& t_input_stream() override;
    virtual CoreOutputStreamInt& t_output_stream() override;
    virtual const CoreInputStreamInt& t_input_stream()const override;
    virtual const CoreOutputStreamInt& t_output_stream()const override;
private:
    CoreInputStreamInt& t_input;
    CoreOutputStreamInt& t_output;
};

class ValCESP : public CoreEnvStreamsProviderInt
{
public:
    ValCESP(std::unique_ptr<CoreInputStreamInt>&& input, std::unique_ptr<CoreOutputStreamInt>&& output);
    ~ValCESP() = default;

    virtual CoreInputStreamInt& t_input_stream() override;
    virtual CoreOutputStreamInt& t_output_stream() override;
    virtual const CoreInputStreamInt& t_input_stream()const override;
    virtual const CoreOutputStreamInt& t_output_stream()const override;
private:
    std::unique_ptr<CoreInputStreamInt> t_input;
    std::unique_ptr<CoreOutputStreamInt> t_output;
};


template<class Input, class Out>
class ValStreamCESP : public CoreEnvStreamsProviderInt
{
public:
    ValStreamCESP(std::unique_ptr<Input>&& input, CoreData::stream_read_mode mode, std::unique_ptr<Out>&& output);
    ~ValStreamCESP() = default;

    virtual CoreInputStreamInt& t_input_stream() override;
    virtual CoreOutputStreamInt& t_output_stream() override;
    virtual const CoreInputStreamInt& t_input_stream()const override;
    virtual const CoreOutputStreamInt& t_output_stream()const override;

    Input& is();
    Out& os();

    const Input& is()const;
    const Out& os()const;
private:
    std::unique_ptr<Input> t_is;
    std::unique_ptr<Out> t_os;

    StdCoreInputStream t_input;
    StdCoreOutputStream t_output;
};










template<class Input, class Out>
inline ValStreamCESP<Input, Out>::ValStreamCESP(std::unique_ptr<Input>&& input, CoreData::stream_read_mode mode, std::unique_ptr<Out>&& output):
    t_is(move(input)),
    t_os(move(output)),
    t_input(*t_is, mode),
    t_output(*t_os)
{
}

template<class Input, class Out>
inline CoreInputStreamInt& ValStreamCESP<Input, Out>::t_input_stream()
{
    return t_input;
}

template<class Input, class Out>
inline CoreOutputStreamInt& ValStreamCESP<Input, Out>::t_output_stream()
{
    return t_output;
}

template<class Input, class Out>
inline const CoreInputStreamInt& ValStreamCESP<Input, Out>::t_input_stream() const
{
    return t_input;
}

template<class Input, class Out>
inline const CoreOutputStreamInt& ValStreamCESP<Input, Out>::t_output_stream() const
{
    return t_output;
}

template<class Input, class Out>
inline Input& ValStreamCESP<Input, Out>::is()
{
    return *t_is;
}

template<class Input, class Out>
inline Out& ValStreamCESP<Input, Out>::os()
{
    return *t_os;
}

template<class Input, class Out>
inline const Input& ValStreamCESP<Input, Out>::is() const
{
    return *t_is;
}

template<class Input, class Out>
inline const Out& ValStreamCESP<Input, Out>::os() const
{
    return *t_os;
}
