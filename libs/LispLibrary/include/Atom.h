#pragma once

#include "BigInt.h"
#include "Number.h"
#include <variant>
#include <string>

class Atom
{
    using symbol = std::string;
public:

    explicit Atom(std::string&& str) noexcept;
    explicit Atom(const std::string& str);

    explicit Atom(Number&& val) noexcept;
    explicit Atom(const Number& val);

    ~Atom();

    bool is_symbol() const;
    bool is_number() const;

    symbol& to_symbol();
    Number& to_number();

    const symbol& to_symbol()const;
    const Number& to_number()const;
private:
    std::variant<symbol, Number, void*> t_data;
};