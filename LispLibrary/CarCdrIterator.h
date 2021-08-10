#pragma once
#include "Cell.h"
#include "SExprsFarm.h"

class CarCdrIterator {
public:
    typedef std::forward_iterator_tag iterator_category;
    typedef Cell value_type;
    typedef Cell* pointer;
    typedef Cell& reference;
    typedef long difference_type;
public:
    explicit CarCdrIterator(SExprsFarm& farm);
    CarCdrIterator(Cell* base, SExprsFarm& farm);
    CarCdrIterator operator++(int);
    CarCdrIterator& operator++();
    Cell& operator*();
    const Cell& operator*()const;
    Cell* operator->();
    const Cell* operator->() const;
    bool operator!=(const CarCdrIterator& rh) const;
    bool operator==(const CarCdrIterator& rh) const;

    //bool empty() const;
    Cell& get_elem();
    const Cell& get_elem()const;
private:
    Cell* t_base;
    std::reference_wrapper<SExprsFarm> t_farm;
    bool t_nil;
};

struct CarCdrConstIterator {
public:
    typedef std::forward_iterator_tag iterator_category;
    typedef Cell value_type;
    typedef Cell* pointer;
    typedef Cell& reference;
    typedef long difference_type;
public:
    explicit CarCdrConstIterator(SExprsFarm& farm);
    CarCdrConstIterator(const Cell* base, SExprsFarm& farm);
    CarCdrConstIterator operator++(int);
    CarCdrConstIterator& operator++();
    const Cell& operator*()const;
    const Cell* operator->() const;
    bool operator!=(const CarCdrConstIterator& rh) const;
    bool operator==(const CarCdrConstIterator& rh) const;

    //bool empty() const;
    const Cell& get_elem()const;
private:
    const Cell* t_base;
    std::reference_wrapper<SExprsFarm> t_farm;
    bool t_nil;
};

class CarCdrIteration {
public:
    CarCdrIteration(Cell& dp, SExprsFarm& farm);

    CarCdrIterator begin();
    CarCdrIterator end();

    CarCdrConstIterator begin()const;
    CarCdrConstIterator end()const;
private:
    Cell* t_base;
    SExprsFarm& t_farm;
};

class CarCdrConstIteration {
public:
    CarCdrConstIteration(const Cell& dp, SExprsFarm& farm);

    CarCdrConstIterator begin()const;
    CarCdrConstIterator end()const;
private:
    const Cell* t_base;
    std::reference_wrapper<SExprsFarm> t_farm;
};