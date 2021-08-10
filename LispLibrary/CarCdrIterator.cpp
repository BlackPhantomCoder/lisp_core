#include "CarCdrIterator.h"
#include "Funcs.h"
using namespace std;

CarCdrIterator::CarCdrIterator(SExprsFarm& farm) :
    t_base(nullptr),
    t_farm(farm),
    t_nil(false)
{
}

CarCdrIterator::CarCdrIterator(Cell* base, SExprsFarm& farm) :
    t_base(base),
    t_farm(farm),
    t_nil((base) ? is_null(*base, farm) : false)
{
}

CarCdrIterator CarCdrIterator::operator++(int)
{
    auto result = *this;
    if (!t_base || t_nil) {
        throw "";
    }
    auto& next = cdr(*t_base);
    if (!is_list(next) || is_null(next,t_farm)) t_base = nullptr;
    else t_base = &next;
    return result;
}

CarCdrIterator& CarCdrIterator::operator++()
{
    if(!t_base || t_nil) {
        throw "";
    }
    auto& next = cdr(*t_base);
    if (!is_list(next) || is_null(next, t_farm)) t_base = nullptr;
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

Cell& CarCdrIterator::get_elem()
{
    if (!t_base) throw;
    return *t_base;
}

const Cell& CarCdrIterator::get_elem() const
{
    if (!t_base) throw;
    return *t_base;
}

CarCdrConstIterator::CarCdrConstIterator(SExprsFarm& farm):
    t_base(nullptr),
    t_farm(farm),
    t_nil(false)
{

}

CarCdrConstIterator::CarCdrConstIterator(const Cell* base, SExprsFarm& farm):
    t_base(base),
    t_farm(farm),
    t_nil((base) ? is_null(*base, farm) : false)
{

}

CarCdrConstIterator CarCdrConstIterator::operator++(int)
{
    auto result = *this;
    if (!t_base || t_nil) {
        throw "";
    }
    const auto& next = cdr(*t_base);
    if (!is_list(next) || is_null(next, t_farm)) t_base = nullptr;
    else t_base = &next;
    return result;
}

CarCdrConstIterator& CarCdrConstIterator::operator++()
{
    if (!t_base || t_nil) {
        throw "";
    }
    const auto& next = cdr(*t_base);
    if (!is_list(next) || is_null(next, t_farm)) t_base = nullptr;
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
    if (!t_base) throw "";
    return *t_base;
}

CarCdrIteration::CarCdrIteration(Cell& dp, SExprsFarm& farm):
    t_base(&dp),
    t_farm(farm)
{

}

CarCdrIterator CarCdrIteration::begin()
{
    return CarCdrIterator(t_base, t_farm);
}

CarCdrIterator CarCdrIteration::end()
{
    return CarCdrIterator(nullptr, t_farm);
}

CarCdrConstIterator CarCdrIteration::begin() const
{
    return CarCdrConstIterator(t_base, t_farm);
}

CarCdrConstIterator CarCdrIteration::end() const
{
    return CarCdrConstIterator(nullptr, t_farm);
}

CarCdrConstIteration::CarCdrConstIteration(const Cell& dp, SExprsFarm& farm) :
    t_base(&dp),
    t_farm(farm)
{
}

CarCdrConstIterator CarCdrConstIteration::begin() const
{
    return CarCdrConstIterator(t_base, t_farm);
}

CarCdrConstIterator CarCdrConstIteration::end() const
{
    return CarCdrConstIterator(nullptr, t_farm);
}
