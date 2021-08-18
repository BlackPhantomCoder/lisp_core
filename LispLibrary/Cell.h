#pragma once
#include "SExpr.h"
#include "CarCdrIterator.h"

class Cell {
    friend class SExprsFarm;
    friend class SymbolsFarm;

    friend bool is_real(const Cell& exp);
    friend bool is_integer(const Cell& exp);
    friend bool is_number(const Cell& exp);
    friend bool is_symbol(const Cell& exp);
    friend bool is_atom(const Cell& exp);
    friend bool is_list(const Cell& exp);
    
    friend const DotPair& to_list(const Cell& exp);
    friend DotPair& to_list(Cell& exp);
    
    friend const Symbol& to_symbol(const Cell& exp);
    friend Symbol& to_symbol(Cell& exp);
    
    friend const Number& to_number(const Cell& exp);
    friend Number& to_number(Cell& exp);
    
    
    friend Cell to_cell(SExprShare core);
public:
    Cell(SExprShare core = {});
     
    Cell(Cell&& rh) noexcept  = default;
    Cell(const Cell& rh) = default;

    Cell& operator=(Cell&& rh) noexcept = default;
    Cell& operator=(const Cell& rh) = default;


    CarCdrIterator begin();
    CarCdrIterator end();

    CarCdrConstIterator begin()const;
    CarCdrConstIterator end()const;

    bool empty() const;
    void clear();
private:
    SExprShare t_data;
};