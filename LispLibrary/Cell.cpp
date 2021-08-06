#include "Cell.h"
#include "DotPair.h"
using namespace std;


Cell::Cell(Cell&& rh)noexcept :
    t_data(std::move(rh.t_data))
{
}

Cell::Cell(const Cell& rh) :
    t_data(rh.t_data)
{
}

Cell::Cell(Atom&& atom) noexcept :
    t_data(make_shared<var_type>(std::move(atom)))
{
}

Cell::Cell(DotPair&& list) noexcept :
    t_data(make_shared<var_type>(std::move(list)))
{
}

Cell::Cell(const Atom& atom) :
    t_data(make_shared<var_type>(atom))
{
}

Cell::Cell(const DotPair& list) :
    t_data(make_shared<var_type>(list))
{
}

Cell& Cell::operator=(const Cell& rh)
{
    if (&rh == this) return *this;
    t_data = rh.t_data;
    return *this;
}

bool Cell::is_atom() const {
    return t_data &&  std::holds_alternative<Atom>(*t_data);
}

bool Cell::is_list() const {
    return t_data && std::holds_alternative<DotPair>(*t_data);
}

Atom& Cell::to_atom() {
    if (!is_atom())throw "to_atom";
    return std::get<Atom>(*t_data);
}

DotPair& Cell::to_list() {
    if (!is_list())throw "to_list";
    return std::get<DotPair>(*t_data);
}

const Atom& Cell::to_atom()const {
    if (!is_atom())throw "to_atom";
    return std::get<Atom>(*t_data);
}

const DotPair& Cell::to_list()const {
    if (!is_list())throw "to_list";
    return std::get<DotPair>(*t_data);
}
