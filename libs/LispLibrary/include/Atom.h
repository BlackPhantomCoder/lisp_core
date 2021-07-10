#pragma once

#include "BigInt.h"

#include <variant>
#include <string>

class Atom
{
    using symbol = std::string;
    using integer = BigInt;
    using real = double;
public:

    explicit Atom(std::string&& str) noexcept;
    explicit Atom(const std::string& str);

    explicit Atom(BigInt&& val) noexcept;
    explicit Atom(const BigInt& val);

    explicit Atom(double val);

    ~Atom();

    bool is_symbol() const;
    bool is_integer() const;
    bool is_real() const;

    symbol& to_symbol();
    integer& to_integer();
    real& to_real();

    const symbol& to_symbol()const;
    const integer& to_integer()const;
    const real& to_real()const;
private:
    std::variant<symbol, integer, real, void*> t_data;
};