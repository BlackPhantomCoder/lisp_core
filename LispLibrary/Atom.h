#pragma once

#include "BigInt.h"
#include "Number.h"
#include "Symbol.h"

#include <variant>

class Atom
{
public:

    explicit Atom(Symbol&& str) noexcept;
    explicit Atom(const Symbol& str);

    explicit Atom(Number&& val) noexcept;
    explicit Atom(const Number& val);

    ~Atom();

    bool is_symbol() const;
    bool is_number() const;

    Symbol& to_symbol();
    Number& to_number();

    const Symbol& to_symbol()const;
    const Number& to_number()const;
private:
    std::variant<Symbol, Number, void*> t_data;
};