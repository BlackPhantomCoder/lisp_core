#pragma once
#include "SExprsFarm.h"

class Cell;
class DotPair
{
    friend class SExprsFarm;
    
    friend Cell& car(Cell& dp);
    friend Cell& cdr(Cell& dp);
    friend const Cell& car(const Cell& dp);
    friend const Cell& cdr(const Cell& dp);

    friend void rplaca(DotPair& rh, const Cell& exp);
    friend void rplacd(DotPair& rh, const Cell& exp);

    friend bool is_null(const DotPair& lst);
public:
    DotPair();
    ~DotPair();

    DotPair(const DotPair& rh) = default;
    DotPair(DotPair&& rh)noexcept;

    DotPair& operator=(DotPair&& rh)noexcept;
    DotPair& operator=(const DotPair& rh)noexcept = default;

    bool empty() const;
    void clear();
private:
    DotPair(SExprsFarm& farm);
    DotPair(const Cell& f, const Cell& s, SExprsFarm& farm);
private:
    Cell t_first;
    Cell t_second;

    SExprsFarm* t_symbols;
};

bool is_null(const DotPair& lst);