#pragma once
#include "DotPair.h"

class CarCdrIterator {
public:
    typedef std::forward_iterator_tag iterator_category;
    typedef Cell value_type;
    typedef Cell* pointer;
    typedef Cell& reference;
    typedef long difference_type;
public:
    explicit CarCdrIterator(DotPair* base = nullptr) : t_base(base) {};
    CarCdrIterator operator++(int);
    CarCdrIterator& operator++();
    Cell& operator*();
    const Cell& operator*()const;
    Cell* operator->();
    const Cell* operator->() const;
    bool operator!=(const CarCdrIterator& rh) const;
    bool operator==(const CarCdrIterator& rh) const;

    DotPair& get_pair();
    const DotPair& get_pair()const;
private:
    DotPair* t_base;
};

struct CarCdrConstIterator {
public:
    typedef std::forward_iterator_tag iterator_category;
    typedef Cell value_type;
    typedef Cell* pointer;
    typedef Cell& reference;
    typedef long difference_type;
public:
    explicit CarCdrConstIterator(const DotPair* base = nullptr);
    CarCdrConstIterator operator++(int);
    CarCdrConstIterator& operator++();
    const Cell& operator*()const;
    const Cell* operator->() const;
    bool operator!=(const CarCdrConstIterator& rh) const;
    bool operator==(const CarCdrConstIterator& rh) const;

    const DotPair& get_pair()const;
private:
    const DotPair* t_base;
};

class CarCdrIteration {
public:
    CarCdrIteration(DotPair& dp);

    CarCdrIterator begin();
    CarCdrIterator end();

    CarCdrConstIterator begin()const;
    CarCdrConstIterator end()const;
private:
    DotPair* t_base;
};

class CarCdrConstIteration {
public:
    CarCdrConstIteration(const DotPair& dp);

    CarCdrConstIterator begin()const;
    CarCdrConstIterator end()const;
private:
    const DotPair* t_base;
};