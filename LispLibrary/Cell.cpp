#include "Cell.h"
using namespace std;

Cell::Cell(SExprCoreShare core):
    t_data(move(core))
{
}

CarCdrIterator Cell::begin()
{
    return CarCdrIterator(this);
}

CarCdrIterator Cell::end()
{
    return CarCdrIterator();
}

CarCdrConstIterator Cell::begin() const
{
    return CarCdrConstIterator(this);
}

CarCdrConstIterator Cell::end() const
{
    return CarCdrConstIterator();
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
    return !exp.empty() && is_real(*exp.t_data);
}

bool is_integer(const Cell& exp)
{
    return !exp.empty() && is_integer(*exp.t_data);
}

bool is_number(const Cell& exp)
{
    return !exp.empty() && is_number(*exp.t_data);
}

bool is_symbol(const Cell& exp)
{
    return !exp.empty() && is_symbol(*exp.t_data);
}

bool is_atom(const Cell& exp)
{
    return !exp.empty() && is_atom(*exp.t_data);
}

bool is_list(const Cell& exp)
{
    return !exp.empty() && is_list(*exp.t_data);
}

const DotPair& to_list(const Cell& exp)
{
    return to_list(*exp.t_data);
}

DotPair& to_list(Cell& exp)
{
    return to_list(*exp.t_data);
}

const Symbol& to_symbol(const Cell& exp)
{
    return to_symbol(*exp.t_data);
}

Symbol& to_symbol(Cell& exp)
{
    return to_symbol(*exp.t_data);
}

const Number& to_number(const Cell& exp)
{
    return to_number(*exp.t_data);
}

Number& to_number(Cell& exp)
{
    return to_number(*exp.t_data);
}

Cell to_cell(SExprCoreShare core)
{
    return Cell(core);
}