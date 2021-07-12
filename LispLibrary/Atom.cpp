#include "Atom.h"

using namespace std;

Atom::Atom(std::string&& str)noexcept:
    t_data(move(str))
{
}

Atom::Atom(const std::string& str):
    t_data(str)
{
}

Atom::Atom(Number&& val) noexcept:
    t_data(move(val))
{
}

Atom::Atom(const Number& val) :
    t_data(val)
{
}


Atom::~Atom()
{
    t_data = nullptr;
}

bool Atom::is_symbol() const
{
    return holds_alternative<Atom::symbol>(t_data);
}

bool Atom::is_number() const
{
    return holds_alternative<Number>(t_data);
}

Atom::symbol& Atom::to_symbol()
{
    if (!is_symbol()) throw "invalid type";
    return get<Atom::symbol>(t_data);
}

Number& Atom::to_number()
{
    if (!is_number()) throw "invalid type";
    return get<Number>(t_data);
}

const Atom::symbol& Atom::to_symbol() const
{
    if (!is_symbol()) throw "invalid type";
    return get<Atom::symbol>(t_data);
}

const Number& Atom::to_number() const
{
    if (!is_number()) throw "invalid type";
    return get<Number>(t_data);
}