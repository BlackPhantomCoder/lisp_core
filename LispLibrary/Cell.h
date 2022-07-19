#pragma once
#include <stdexcept>
#include "SExpr.h"

enum class cell_type {numb, symbol, list};

class Cell {
    friend class SExprsFarm;
    friend class SymbolsFarm;

    friend cell_type type(const Cell& exp);
    template<cell_type T>
    friend auto& to_type(const Cell& exp);

    friend bool is_real(const Cell& exp);
    friend bool is_integer(const Cell& exp);
    friend bool is_atom(const Cell& exp);

    friend bool is_number(const Cell& exp);
    friend bool is_symbol(const Cell& exp);
    
    friend bool is_list(const Cell& exp);
    
    friend const DotPair& to_list(const Cell& exp);
    friend DotPair& to_list(Cell& exp);
    
    friend const Symbol& to_symbol(const Cell& exp);
    friend Symbol& to_symbol(Cell& exp);
    
    friend const Number& to_number(const Cell& exp);
    friend Number& to_number(Cell& exp);
    
    
    friend Cell to_cell(SExprShare core);
public:
    Cell(SExprShare&& core = {});
    Cell(const SExprShare& core);
     
    Cell(Cell&& rh) noexcept  = default;
    Cell(const Cell& rh) = default;

    Cell& operator=(Cell&& rh) noexcept = default;
    Cell& operator=(const Cell& rh) = default;

    bool empty() const;
    void clear();
private:
    SExprShare t_data;
};











template<cell_type T>
auto& to_type(const Cell& exp) {
    switch (type(exp))
    {
    case cell_type::list:
        return exp.t_data->to_list();
    case cell_type::numb:
        return exp.t_data->to_number();
    case cell_type::symbol:
        return exp.t_data->to_symbol();
    default:
        throw std::logic_error("to_type");
    }
}