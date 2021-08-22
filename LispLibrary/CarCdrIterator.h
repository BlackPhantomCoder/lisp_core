#pragma once
#include <iterator>

class Cell;
class CarCdrConstIterator;
class CarCdrIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Cell ;
    using pointer = Cell*;
    using reference = Cell&;
    using const_reference = const Cell&;
    using difference_type = long;
public:
    CarCdrIterator(CarCdrIterator&& rh) noexcept;
    CarCdrIterator& operator=(CarCdrIterator&& rh) noexcept;

    CarCdrIterator(const CarCdrIterator& rh) = default;
    CarCdrIterator& operator=(const CarCdrIterator& rh) = default;

    CarCdrIterator();
    CarCdrIterator(Cell& base);
    CarCdrIterator operator++(int);
    CarCdrIterator& operator++();
    reference operator*();
    const_reference operator*()const;
    bool operator!=(const CarCdrIterator& rh) const;
    bool operator==(const CarCdrIterator& rh) const;

    operator CarCdrConstIterator() const;

    reference get_elem();
    const_reference get_elem()const;
private:
    Cell* t_base;
    bool t_nil;
};

class CarCdrConstIterator {
    friend class CarCdrIterator;
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Cell;
    using pointer = Cell*;
    using reference = Cell&;
    using const_reference = const Cell&;
    using difference_type = long;
public:
    CarCdrConstIterator(CarCdrConstIterator&& rh) noexcept;
    CarCdrConstIterator& operator=(CarCdrConstIterator&& rh) noexcept;

    CarCdrConstIterator(const CarCdrConstIterator& rh) = default;
    CarCdrConstIterator& operator=(const CarCdrConstIterator& rh) = default;

    CarCdrConstIterator();
    CarCdrConstIterator(const Cell& base);
    CarCdrConstIterator operator++(int);
    CarCdrConstIterator& operator++();
    const_reference operator*()const;
    bool operator!=(const CarCdrConstIterator& rh) const;
    bool operator==(const CarCdrConstIterator& rh) const;
    const_reference get_elem()const;
private:
    const Cell* t_base;
    bool t_nil;
};

CarCdrIterator begin(Cell& c);
CarCdrIterator end(Cell& c);

CarCdrConstIterator begin(const Cell& c);
CarCdrConstIterator end(const Cell& c);