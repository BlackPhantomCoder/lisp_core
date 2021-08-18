#pragma once
#include "SExpr.h"
#include "BigInt.h"

#include <variant>
#include <string>

class Number : public SExpr
{
public:
    using integer = BigInt;
    using real = double;


private:
    friend class SExprsFarm;

    friend bool is_integer(const Number& numb);
    friend bool is_real(const Number& numb);

    friend Number::integer& to_integer(Number& numb);
    friend Number::real& to_real(Number& numb);
    friend const Number::integer& to_integer(const Number& numb);
    friend const Number::real& to_real(const Number& numb);

    virtual SExpr::del_func_ptr get_del_fnc() const override;
public:
    static unsigned epsilon;
public:
    Number() = default;
    ~Number();

    Number(Number&& rh)noexcept;
    Number(const Number& rh);

    Number& operator=(Number&& rh)noexcept;
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

    std::string to_string()const;

    bool empty() const;
    void clear();

    virtual bool is_real() const override;
    virtual bool is_integer() const override;
    virtual bool is_number() const override;
    virtual bool is_atom() const override;

    //virtual const Number& to_number() const override;
    //virtual Number& to_number() override;
private:
    Number(double val);
    Number(const BigInt& val);

    bool t_is_integer() const;
    bool t_is_real() const;

    const Number::integer& to_integer() const;
    const Number::real& to_real() const;

    Number::integer& to_integer() ;
    Number::real& to_real() ;
private:
    std::variant<integer, real, std::monostate> t_data = std::monostate{};
};

bool is_integer(const Number& numb);
bool is_real(const Number& numb);

Number::integer& to_integer(Number& numb);
Number::real& to_real(Number& numb);
const Number::integer& to_integer(const Number& numb);
const Number::real& to_real(const Number& numb);