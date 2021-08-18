#include "CarCdrIterator.h"
#include "Funcs.h"
using namespace std;

CarCdrIterator::CarCdrIterator(CarCdrIterator&& rh) noexcept :
    t_base(rh.t_base),
    t_nil(rh.t_nil)
{
    rh.t_base = nullptr;
    rh.t_nil = false;
}

CarCdrIterator& CarCdrIterator::operator=(CarCdrIterator&& rh) noexcept
{
    if (this != &rh) {
        t_base = rh.t_base;
        t_nil = rh.t_nil;

        rh.t_base = nullptr;
        rh.t_nil = false;
    }
    return *this;
}

CarCdrIterator::CarCdrIterator() :
    t_base(nullptr),
    t_nil(false)
{
}

CarCdrIterator::CarCdrIterator(Cell* base) :
    t_base(base),
    t_nil((base) ? is_null(*base) : false)
{
}

CarCdrIterator CarCdrIterator::operator++(int)
{
    auto result = *this;
    if (!t_base || t_nil) {
        throw "";
    }
    auto& next = cdr(*t_base);
    if (!is_list(next) || is_null_list(to_list(next))) t_base = nullptr;
    else t_base = &next;
    return result;
}

CarCdrIterator& CarCdrIterator::operator++()
{
    if(!t_base || t_nil) {
        throw "";
    }
    auto& next = cdr(*t_base);
    if (!is_list(next) || is_null_list(to_list(next))) t_base = nullptr;
    else t_base = &next;
    return *this;
}

Cell& CarCdrIterator::operator*()
{
    if (!t_base) {
        throw "";
    }
    return car(*t_base);
}

const Cell& CarCdrIterator::operator*() const
{
    if (!t_base) {
        throw "";
    }
    return car(*t_base);
}

Cell* CarCdrIterator::operator->()
{
    if (!t_base) {
        throw "";
    }
    return &car(*t_base);
}

const Cell* CarCdrIterator::operator->() const
{
    if (!t_base) {
        throw "";
    }
    return &car(*t_base);
}

bool CarCdrIterator::operator!=(const CarCdrIterator& rh) const
{
    return !(*this == rh);
}

bool CarCdrIterator::operator==(const CarCdrIterator& rh) const
{
    return (t_base && t_nil && !rh.t_base) || t_base == rh.t_base;
}

CarCdrIterator::operator CarCdrConstIterator() const
{
    CarCdrConstIterator buf;
    buf.t_base = t_base;
    buf.t_nil = t_nil;
    return buf;
}

Cell& CarCdrIterator::get_elem()
{
    if (!t_base) {
        throw "";
    }
    return *t_base;
}

const Cell& CarCdrIterator::get_elem() const
{
    if (!t_base) {
        throw "";
    }
    return *t_base;
}

CarCdrConstIterator::CarCdrConstIterator(CarCdrConstIterator&& rh) noexcept :
    t_base(rh.t_base),
    t_nil(rh.t_nil)
{
    rh.t_base = nullptr;
    rh.t_nil = false;
}

CarCdrConstIterator& CarCdrConstIterator::operator=(CarCdrConstIterator&& rh) noexcept
{
    if (this != &rh) {
        t_base = rh.t_base;
        t_nil = rh.t_nil;

        rh.t_base = nullptr;
        rh.t_nil = false;
    }
    return *this;
}


CarCdrConstIterator::CarCdrConstIterator():
    t_base(nullptr),
    t_nil(false)
{
}

CarCdrConstIterator::CarCdrConstIterator(const Cell* base):
    t_base(base),
    t_nil((base) ? is_null(*base) : false)
{

}

CarCdrConstIterator CarCdrConstIterator::operator++(int)
{
    auto result = *this;
    if (!t_base || t_nil) {
        throw "";
    }
    const auto& next = cdr(*t_base);
    if (!is_list(next) || is_null_list(to_list(next))) t_base = nullptr;
    else t_base = &next;
    return result;
}

CarCdrConstIterator& CarCdrConstIterator::operator++()
{
    if (!t_base || t_nil) {
        throw "";
    }
    const auto& next = cdr(*t_base);
    if (!is_list(next) || is_null_list(to_list(next))) t_base = nullptr;
    else t_base = &next;
    return *this;
}

const Cell& CarCdrConstIterator::operator*() const
{
    if (!t_base) {
        throw "";
    }
    return car(*t_base);
}

const Cell* CarCdrConstIterator::operator->() const
{
    if (!t_base) {
        throw "";
    }
    return &car(*t_base);
}

bool CarCdrConstIterator::operator!=(const CarCdrConstIterator& rh) const
{
    return !(*this == rh);
}

bool CarCdrConstIterator::operator==(const CarCdrConstIterator& rh) const
{
    return (t_base && t_nil && !rh.t_base) || t_base == rh.t_base;
}

const Cell& CarCdrConstIterator::get_elem() const
{
    if (!t_base) {
        throw "";
    }
    return *t_base;
}