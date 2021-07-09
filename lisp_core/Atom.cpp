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

Atom::Atom(BigInt&& val)noexcept :
    t_data(move(val))
{
}

Atom::Atom(const BigInt& val) :
    t_data(val)
{
}

Atom::Atom(double val) :
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

bool Atom::is_integer() const
{
    return holds_alternative<Atom::integer>(t_data);
}

bool Atom::is_real() const
{
    return holds_alternative<Atom::real>(t_data);
}

Atom::symbol& Atom::to_symbol()
{
    if (!is_symbol()) throw "invalid type";
    return get<Atom::symbol>(t_data);
}

Atom::integer& Atom::to_integer()
{
    if (!is_integer()) throw "invalid type";
    return get<Atom::integer>(t_data);
}

Atom::real& Atom::to_real()
{
    if (!is_real()) throw "invalid type";
    return get<Atom::real>(t_data);
}

const Atom::symbol& Atom::to_symbol() const
{
    if (!is_symbol()) throw "invalid type";
    return get<Atom::symbol>(t_data);
}

const Atom::integer& Atom::to_integer() const
{
    if (!is_integer()) throw "invalid type";
    return get<Atom::integer>(t_data);
}

const Atom::real& Atom::to_real() const
{
    if (!is_real()) throw "invalid type";
    return get<Atom::real>(t_data);
}
