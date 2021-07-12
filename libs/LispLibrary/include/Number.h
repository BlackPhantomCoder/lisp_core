#pragma once
#include "BigInt.h"

#include <variant>
#include <string>


class Number
{
public:
    using integer = BigInt;
    using real = double;

public:
    static unsigned epsilon;
public:
    Number(Number&& rh);
    Number(const Number& rh);

    explicit Number(double val);
    explicit Number(const BigInt& val);

    Number& operator=(Number&& rh);
    Number& operator=(const Number& rh);

    bool operator<(const Number& rh)const;
    bool operator>(const Number& rh)const;
    bool operator==(const Number& rh)const;

    bool operator<=(const Number& rh)const;
    bool operator>=(const Number& rh)const;

    Number& operator+=(const Number& rh);
    Number& operator-=(const Number& rh);
    Number& operator*=(const Number& rh);
    Number& operator/=(const Number& rh);

    Number& div(const Number& rh);
    Number mod(const Number& rh) const;

    void minus();
    void abs();

    bool is_integer() const;
    bool is_real() const;

    const integer& to_integer()const;
    const real& to_real()const;

    std::string to_string()const;
private:
    std::variant<integer, real, void*> t_data;
};