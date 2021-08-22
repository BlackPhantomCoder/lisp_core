#pragma once
#include "CoreStreamInt.h"
#include <iostream>
#include <vector>

//stream array by reference
template <class Array>
class StdCoreInputReferenceArrayInput : public CoreInputStreamInt {
public:
	StdCoreInputReferenceArrayInput(const Array& arr, CoreData::stream_read_mode mode);
	StdCoreInputReferenceArrayInput(const StdCoreInputReferenceArrayInput& rh);
	virtual bool alive() const;
	virtual bool ready() const override;


	virtual CoreData::stream_read_mode get_mode() const override;
	void rewind();

protected:
	virtual std::string t_read_line() override;
private:
    std::reference_wrapper<const Array> t_lines;
    typename Array::const_iterator t_pos;
	CoreData::stream_read_mode t_mode;
};


//stream array by val
template <class Array>
class StdCoreInputValArrayInput : public CoreInputStreamInt {
public:
    StdCoreInputValArrayInput(Array&& arr, CoreData::stream_read_mode mode);
    StdCoreInputValArrayInput(const Array& arr, CoreData::stream_read_mode mode);
    StdCoreInputValArrayInput(const StdCoreInputValArrayInput& rh);
    virtual bool alive() const;
    virtual bool ready() const override;


    virtual CoreData::stream_read_mode get_mode() const override;
    void rewind();

protected:
    virtual std::string t_read_line() override;
private:
    Array t_lines;
    typename Array::const_iterator t_pos;
    CoreData::stream_read_mode t_mode;
};

template <class Array>
StdCoreInputReferenceArrayInput<Array>::StdCoreInputReferenceArrayInput(const Array& arr, CoreData::stream_read_mode mode) :
    t_mode(mode),
    t_lines(arr)
{
    t_pos = (std::begin(t_lines));
}

template <class Array>
StdCoreInputReferenceArrayInput<Array>::StdCoreInputReferenceArrayInput(const StdCoreInputReferenceArrayInput& rh) :
    t_mode(rh.t_mode)
{
    t_lines = rh.t_lines;
    t_pos = std::begin(t_lines) + std::distance(std::begin(rh.t_lines), rh.t_pos);
}

template <class Array>
bool StdCoreInputReferenceArrayInput<Array>::alive() const
{
    return t_pos != std::end(t_lines);
}

template <class Array>
bool StdCoreInputReferenceArrayInput<Array>::ready() const
{
    return alive();
}

template <class Array>
std::string StdCoreInputReferenceArrayInput<Array>::t_read_line()
{
    if (t_pos == std::end(t_lines)) throw "StdCoreInputReferenceArrayInput::t_read_line: out of range";
    std::string buf = *t_pos;
    ++t_pos;
    return buf;
}

template <class Array>
CoreData::stream_read_mode StdCoreInputReferenceArrayInput<Array>::get_mode() const
{
    return t_mode;
}

template <class Array>
void StdCoreInputReferenceArrayInput<Array>::rewind()
{
    t_clear_last();
    t_pos = std::begin(t_lines);
}



template <class Array>
StdCoreInputValArrayInput<Array>::StdCoreInputValArrayInput(Array&& arr, CoreData::stream_read_mode mode) :
    t_mode(mode),
    t_lines(std::move(arr))
{
    t_pos = (std::begin(t_lines));
}


template <class Array>
StdCoreInputValArrayInput<Array>::StdCoreInputValArrayInput(const Array& arr, CoreData::stream_read_mode mode) :
    t_mode(mode),
    t_lines(arr)
{
    t_pos = (std::begin(t_lines));
}

template <class Array>
StdCoreInputValArrayInput<Array>::StdCoreInputValArrayInput(const StdCoreInputValArrayInput& rh) :
    t_mode(rh.t_mode)
{
    t_lines = rh.t_lines;
    t_pos = std::begin(t_lines) + std::distance(std::begin(rh.t_lines), rh.t_pos);
}

template <class Array>
bool StdCoreInputValArrayInput<Array>::alive() const
{
    return t_pos != std::end(t_lines);
}

template <class Array>
bool StdCoreInputValArrayInput<Array>::ready() const
{
    return alive();
}

template <class Array>
std::string StdCoreInputValArrayInput<Array>::t_read_line()
{
    if (t_pos == std::end(t_lines)) throw "StdCoreInputValArrayInput::t_read_line: out of range";
    std::string buf = *t_pos;
    ++t_pos;
    return buf;
}

template <class Array>
CoreData::stream_read_mode StdCoreInputValArrayInput<Array>::get_mode() const
{
    return t_mode;
}

template <class Array>
void StdCoreInputValArrayInput<Array>::rewind()
{
    t_clear_last();
    t_pos = std::begin(t_lines);
}