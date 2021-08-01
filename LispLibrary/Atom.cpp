#include "Atom.h"

using namespace std;

Atom::Atom(Symbol&& str)noexcept:
    t_data(move(str))
{
}

Atom::Atom(const Symbol& str):
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
    return holds_alternative<Symbol>(t_data);
}

bool Atom::is_number() const
{
    return holds_alternative<Number>(t_data);
}

Symbol& Atom::to_symbol()
{
    if (!is_symbol()) throw "invalid type";
    return get<Symbol>(t_data);
}

Number& Atom::to_number()
{
    if (!is_number()) throw "invalid type";
    return get<Number>(t_data);
}

const Symbol& Atom::to_symbol() const
{
    if (!is_symbol()) throw "invalid type";
    return get<Symbol>(t_data);
}

const Number& Atom::to_number() const
{
    if (!is_number()) throw "invalid type";
    return get<Number>(t_data);
}
