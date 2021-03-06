#pragma once
#include "SExpr.h"
#include "CoreData.h"

class DotPair : public SExpr
{
    friend class SExprsFarm;
    friend class CarCdrIterator;
    friend class CarCdrConstIterator;
    
    friend Cell& car(Cell& dp);
    friend Cell& cdr(Cell& dp);
    friend const Cell& car(const Cell& dp);
    friend const Cell& cdr(const Cell& dp);

    friend void rplaca(DotPair& rh, const Cell& exp);
    friend void rplacd(DotPair& rh, const Cell& exp);

    friend bool is_null_list(const DotPair& lst);
public:
    DotPair();
    DotPair(const Cell& f, const Cell& s);
    ~DotPair() = default;

    DotPair(const DotPair& rh);
    DotPair(DotPair&& rh)noexcept;

    DotPair& operator=(DotPair&& rh)noexcept;
    DotPair& operator=(const DotPair& rh)noexcept;

    bool empty() const;

    void clear();
private:
    Cell t_first;
    Cell t_second;
};

bool is_null_list(const DotPair& lst);
