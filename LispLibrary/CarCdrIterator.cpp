#include "CarCdrIterator.h"
#include "Funcs.h"
using namespace std;

CarCdrIterator CarCdrIterator::operator++(int)
{
    auto result = *this;
    if (!t_base || is_null(*t_base)) {
        throw "";
    }
    auto& next = cdr(*t_base);
    if (!is_list(next) || is_null(to_list(next))) t_base = nullptr;
    else t_base = &to_list(next);
    return result;
}

CarCdrIterator& CarCdrIterator::operator++()
{
    if(!t_base || is_null(*t_base)) {
        throw "";
    }
    auto& next = cdr(*t_base);
    if (!is_list(next) || is_null(to_list(next))) t_base = nullptr;
    else t_base = &to_list(next);
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
    return (t_base && empty(*t_base) && !rh.t_base) || t_base == rh.t_base;
}

DotPair& CarCdrIterator::get_pair()
{
    if (!t_base) throw;
    return *t_base;
}

const DotPair& CarCdrIterator::get_pair() const
{
    if (!t_base) throw;
    return *t_base;
}

CarCdrConstIterator::CarCdrConstIterator(const DotPair* base)
    : t_base(base) 
{
};

CarCdrConstIterator CarCdrConstIterator::operator++(int)
{
    auto result = *this;
    if (!t_base || is_null(*t_base)) {
        throw "";
    }
    const auto& next = cdr(*t_base);
    if (!is_list(next) || is_null(to_list(next))) t_base = nullptr;
    else t_base = &to_list(next);
    return result;
}

CarCdrConstIterator& CarCdrConstIterator::operator++()
{
    if (!t_base || is_null(*t_base)) {
        throw "";
    }
    const auto& next = cdr(*t_base);
    if (!is_list(next) || is_null(to_list(next))) t_base = nullptr;
    else t_base = &to_list(next);
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
    return (t_base && empty(*t_base) && !rh.t_base) || t_base == rh.t_base;
}

const DotPair& CarCdrConstIterator::get_pair() const
{
    if (!t_base) throw;
    return *t_base;
}

CarCdrIteration::CarCdrIteration(DotPair& dp):
    t_base(&dp)
{
}

CarCdrIterator CarCdrIteration::begin()
{
    return CarCdrIterator(t_base);
}

CarCdrIterator CarCdrIteration::end()
{
    return CarCdrIterator(nullptr);
}

CarCdrConstIterator CarCdrIteration::begin() const
{
    return CarCdrConstIterator(t_base);
}

CarCdrConstIterator CarCdrIteration::end() const
{
    return CarCdrConstIterator(nullptr);
}

CarCdrConstIteration::CarCdrConstIteration(const DotPair& dp):
    t_base(&dp)
{
}

CarCdrConstIterator CarCdrConstIteration::begin() const
{
    return CarCdrConstIterator(t_base);
}

CarCdrConstIterator CarCdrConstIteration::end() const
{
    return CarCdrConstIterator(nullptr);
}
