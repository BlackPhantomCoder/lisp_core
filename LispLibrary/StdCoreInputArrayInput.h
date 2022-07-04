#pragma once
#include "CoreStreamInt.h"
#include <iostream>
#include <vector>

////stream array by reference
//template <class Array>
//class StdCoreInputReferenceArrayInput : public CoreInputStreamInt {
//public:
//	StdCoreInputReferenceArrayInput(const Array& arr, CoreData::stream_read_mode mode);
//	StdCoreInputReferenceArrayInput(const StdCoreInputReferenceArrayInput& rh);
//	virtual bool alive() const override;
//
//	virtual CoreData::stream_read_mode get_mode() const override;
//	void rewind();
//
//protected:
//	virtual char t_read_char() override;
//private:
//    std::reference_wrapper<const Array> t_lines;
//    typename Array::const_iterator t_pos;
//	CoreData::stream_read_mode t_mode;
//};


//stream array by val
template <class Array>
class StdCoreInputValArrayInput : public CoreInputStreamInt {
public:
    StdCoreInputValArrayInput(Array&& arr, CoreData::stream_read_mode mode);
    StdCoreInputValArrayInput(const Array& arr, CoreData::stream_read_mode mode);
    StdCoreInputValArrayInput(const StdCoreInputValArrayInput& rh);

    virtual CoreData::stream_read_mode get_mode() const override;
    void rewind();

protected:
    virtual int t_read_char() override;
    virtual int t_peek_char() override;
    virtual bool t_alive() const override;
private:
    Array t_lines;
    typename Array::const_iterator t_pos;
    size_t t_pos_at_str = 0;
    CoreData::stream_read_mode t_mode;
    bool t_eos = false;;
};

//template <class Array>
//StdCoreInputReferenceArrayInput<Array>::StdCoreInputReferenceArrayInput(const Array& arr, CoreData::stream_read_mode mode) :
//    t_mode(mode),
//    t_lines(arr)
//{
//    t_pos = (std::begin(t_lines));
//}
//
//template <class Array>
//StdCoreInputReferenceArrayInput<Array>::StdCoreInputReferenceArrayInput(const StdCoreInputReferenceArrayInput& rh) :
//    t_mode(rh.t_mode)
//{
//    t_lines = rh.t_lines;
//    t_pos = std::begin(t_lines) + std::distance(std::begin(rh.t_lines), rh.t_pos);
//}
//
//template <class Array>
//bool StdCoreInputReferenceArrayInput<Array>::alive() const
//{
//    return t_pos != std::end(t_lines);
//}
//
//template <class Array>
//char StdCoreInputReferenceArrayInput<Array>::t_read_char()
//{
//    if (t_pos == std::end(t_lines)) throw "StdCoreInputReferenceArrayInput::t_read_char: out of range";
//    std::string buf = *t_pos;
//    ++t_pos;
//    return buf;
//}
//
//
//template <class Array>
//CoreData::stream_read_mode StdCoreInputReferenceArrayInput<Array>::get_mode() const
//{
//    return t_mode;
//}
//
//template <class Array>
//void StdCoreInputReferenceArrayInput<Array>::rewind()
//{
//    t_clear_last();
//    t_pos = std::begin(t_lines);
//}



template <class Array>
StdCoreInputValArrayInput<Array>::StdCoreInputValArrayInput(Array&& arr, CoreData::stream_read_mode mode) :
    t_mode(mode),
    t_lines(std::move(arr))
{
    for (auto& str : t_lines) {
        str += '\n';
    }
    t_lines.back().pop_back();
    t_pos = std::begin(t_lines);
    t_pos_at_str = 0;
    while (t_pos != std::end(t_lines) && std::empty(*t_pos)) {
        ++t_pos;
    }
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
bool StdCoreInputValArrayInput<Array>::t_alive() const
{
    if (t_pos == std::end(t_lines) && t_eos) {
        return false;
    }
    return true;

    //return t_pos != std::end(t_lines);
}
template <class Array>
int StdCoreInputValArrayInput<Array>::t_read_char()
{
    if (t_pos == std::end(t_lines))
    {
        if (!t_eos) {
            t_eos = true;
            return -1;
        }
        throw "StdCoreInputValArrayInput::t_read_char: out of range";
    }
    auto ch = (*t_pos)[t_pos_at_str++];
    if (t_pos_at_str == t_pos->size()) {
        t_pos_at_str = 0;
        ++t_pos;
        while (t_pos != std::end(t_lines) && std::empty(*t_pos)) {
            ++t_pos;
        }
    }
    
    return (unsigned char)ch;
}

template<class Array>
inline int StdCoreInputValArrayInput<Array>::t_peek_char()
{
    if (t_pos == std::end(t_lines))
    {
        if (!t_eos) {
            return -1;
        }
        throw "StdCoreInputValArrayInput::t_peek_char: out of range";
    }
    return (unsigned char)(*t_pos)[t_pos_at_str];
}

template <class Array>
CoreData::stream_read_mode StdCoreInputValArrayInput<Array>::get_mode() const
{
    return t_mode;
}

template <class Array>
void StdCoreInputValArrayInput<Array>::rewind()
{
    t_pos = std::begin(t_lines);
    t_pos_at_str = 0;
    t_eos = false;
}