#pragma once
#include "SExprCore.h"

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
    
    
    friend Cell to_cell(SExprCoreShare core);
public:
    Cell(SExprCoreShare core = {});
     
    Cell(Cell&& rh) noexcept  = default;
    Cell(const Cell& rh) = default;

    Cell& operator=(Cell&& rh) noexcept = default;
    Cell& operator=(const Cell& rh) = default;

    bool empty() const;
    void clear();
private:
    SExprCoreShare t_data;
};