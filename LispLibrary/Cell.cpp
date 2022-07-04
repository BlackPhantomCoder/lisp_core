#include "Cell.h"
#include "Symbol.h"
#include "Number.h"
#include "DotPair.h"
using namespace std;

cell_type type(const Cell& exp) {
    if (exp.t_data->is_list()) return cell_type::list;
    if (exp.t_data->is_number()) return cell_type::numb;
    if (exp.t_data->is_symbol()) return cell_type::symbol;
    throw std::logic_error("type");
}



Cell::Cell(SExprShare core):
    t_data(move(core))
{
}

bool Cell::empty() const
{
    return !t_data;
}

void Cell::clear()
{
    t_data.reset();
}

bool is_real(const Cell& exp)
{
    return !exp.empty() && exp.t_data->is_real();
}

bool is_integer(const Cell& exp)
{
    return !exp.empty() && exp.t_data->is_integer();
}

bool is_number(const Cell& exp)
{
    return !exp.empty() && exp.t_data->is_number();
}

bool is_symbol(const Cell& exp)
{
    return !exp.empty() && exp.t_data->is_symbol();
}

bool is_atom(const Cell& exp)
{
    return !exp.empty() && exp.t_data->is_atom();
}

bool is_list(const Cell& exp)
{
    return !exp.empty() && exp.t_data->is_list();
}

const DotPair& to_list(const Cell& exp)
{
    return exp.t_data->to_list();
}

DotPair& to_list(Cell& exp)
{
    return exp.t_data->to_list();
}

const Symbol& to_symbol(const Cell& exp)
{
    return exp.t_data->to_symbol();
}

Symbol& to_symbol(Cell& exp)
{
    return exp.t_data->to_symbol();
}

const Number& to_number(const Cell& exp)
{
    return exp.t_data->to_number();
}

Number& to_number(Cell& exp)
{
    return exp.t_data->to_number();
}

Cell to_cell(SExprShare core)
{
    return Cell(core);
}