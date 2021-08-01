#include "Cell.h"
using namespace std;

Cell::Cell(Cell&& rh)noexcept :
    t_data(std::move(rh.t_data))
{
}

Cell::Cell(const Cell& rh) :
    t_data(rh.t_data)
{
}

Cell::Cell(oatom&& atom) noexcept :
    t_data(make_shared<var_type>(std::move(atom)))
{
}

Cell::Cell(olist&& list) noexcept :
    t_data(make_shared<var_type>(std::move(list)))
{
}

Cell::Cell(const oatom& atom) :
    t_data(make_shared<var_type>(atom))
{
}

Cell::Cell(const olist& list) :
    t_data(make_shared<var_type>(list))
{
}

Cell::~Cell() {
    t_data = nullptr;
}

Cell& Cell::operator=(const Cell& rh)
{
    if (&rh == this) return *this;
    t_data = rh.t_data;
    return *this;
}

bool Cell::is_atom() const {
    return t_data &&  std::holds_alternative<oatom>(*t_data); 
}

bool Cell::is_list() const {
    return !is_atom(); 
}

Cell::oatom& Cell::to_atom() {
    if (!is_atom())throw "to_atom";
    return std::get<oatom>(*t_data); 
}

Cell::olist& Cell::to_list() {
    if (!is_list())throw "to_list";
    return std::get<olist>(*t_data); 
}

const Cell::oatom& Cell::to_atom()const {
    if (!is_atom())throw "to_atom";
    return std::get<oatom>(*t_data); 
}

const Cell::olist& Cell::to_list()const {
    if (!is_list())throw "to_list";
    return std::get<olist>(*t_data); 
}