#pragma once
#include <iterator>

class Cell;
class CarCdrConstIterator;
class CarCdrIterator {
public:
    typedef std::forward_iterator_tag iterator_category;
    typedef Cell& value_type;
    typedef Cell* pointer;
    typedef Cell& reference;
    typedef long difference_type;
public:
    CarCdrIterator(CarCdrIterator&& rh) noexcept;
    CarCdrIterator& operator=(CarCdrIterator&& rh) noexcept;

    CarCdrIterator(const CarCdrIterator& rh) = default;
    CarCdrIterator& operator=(const CarCdrIterator& rh) = default;

    CarCdrIterator();
    CarCdrIterator(Cell* base);
    CarCdrIterator operator++(int);
    CarCdrIterator& operator++();
    Cell& operator*();
    const Cell& operator*()const;
    Cell* operator->();
    const Cell* operator->() const;
    bool operator!=(const CarCdrIterator& rh) const;
    bool operator==(const CarCdrIterator& rh) const;

    operator CarCdrConstIterator() const;
    //bool empty() const;
    Cell& get_elem();
    const Cell& get_elem()const;
private:
    Cell* t_base;
    bool t_nil;
};

class CarCdrConstIterator {
    friend class CarCdrIterator;
public:
    typedef std::forward_iterator_tag iterator_category;
    typedef Cell& value_type;
    typedef Cell* pointer;
    typedef Cell& reference;
    typedef long difference_type;
public:
    CarCdrConstIterator(CarCdrConstIterator&& rh) noexcept;
    CarCdrConstIterator& operator=(CarCdrConstIterator&& rh) noexcept;

    CarCdrConstIterator(const CarCdrConstIterator& rh) = default;
    CarCdrConstIterator& operator=(const CarCdrConstIterator& rh) = default;

    CarCdrConstIterator();
    CarCdrConstIterator(const Cell* base);
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
    bool t_nil;
};

//class CarCdrIteration {
//public:
//    CarCdrIteration(Cell& dp, SExprsFarm& farm);
//
//    CarCdrIterator begin();
//    CarCdrIterator end();
//
//    CarCdrConstIterator begin()const;
//    CarCdrConstIterator end()const;
//
//    CarCdrIteration& operator=(CarCdrIteration&& rh) noexcept;
//private:
//    Cell* t_base;
//    std::reference_wrapper<SExprsFarm> t_farm;
//};
//
//class CarCdrConstIteration {
//public:
//    CarCdrConstIteration(const Cell& dp, SExprsFarm& farm);
//
//    CarCdrConstIterator begin()const;
//    CarCdrConstIterator end()const;
//private:
//    const Cell* t_base;
//    std::reference_wrapper<SExprsFarm> t_farm;
//};