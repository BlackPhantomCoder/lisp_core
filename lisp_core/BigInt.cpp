#include "BigInt.h"

#include <numeric>

BigInt::BigInt(const BigInt& rh)
{
    mpz_init_set(t_val, rh.t_val);
}

BigInt::BigInt(const std::string& str)
{
    mpz_init_set_str(t_val, str.c_str(), 10);
}

BigInt::BigInt(long n)
{
    mpz_init_set_si(t_val, n);
}

BigInt::BigInt(double n)
{
    mpz_init_set_d(t_val, n);
}

BigInt::~BigInt()
{
    mpz_clear(t_val);
}

BigInt& BigInt::operator=(const BigInt& rh)
{
    mpz_clear(t_val);
    mpz_init_set(t_val, rh.t_val);
    return *this;
}

bool BigInt::operator<(const BigInt& rh) const
{
    return (mpz_cmp(t_val, rh.t_val) < 0);
}

bool BigInt::operator>(const BigInt& rh) const
{
    return (mpz_cmp(t_val, rh.t_val) > 0);
}

bool BigInt::operator==(const BigInt& rh) const
{
    return (mpz_cmp(t_val, rh.t_val) == 0);
}

bool BigInt::operator<=(const BigInt& rh) const
{
    auto buf = mpz_cmp(t_val, rh.t_val);
    return buf == 0 || buf < 0;
}

bool BigInt::operator>=(const BigInt& rh) const
{
    auto buf = mpz_cmp(t_val, rh.t_val);
    return buf == 0 || buf > 0;
}

BigInt& BigInt::operator+=(const BigInt& rh)
{
    mpz_add(t_val, t_val, rh.t_val);
    return *this;
}

BigInt& BigInt::operator-=(const BigInt& rh)
{
    mpz_sub(t_val, t_val, rh.t_val);
    return *this;
}

BigInt& BigInt::operator*=(const BigInt& rh)
{
    mpz_mul(t_val, t_val, rh.t_val);
    return *this;
}

BigInt& BigInt::div(const BigInt& rh)
{
    mpz_div(t_val, t_val, rh.t_val);
    return *this;
}

BigInt& BigInt::mod(const BigInt& rh)
{
    mpz_mod(t_val, t_val, rh.t_val);
    return *this;
}

void BigInt::minus()
{
    mpz_neg(t_val, t_val);
}

void BigInt::abs()
{
    mpz_abs(t_val, t_val);
}

bool BigInt::is_castable_to_double() const
{
    mpz_t buf;
    mpz_init_set_d(buf, std::numeric_limits<double>::max());
    auto result = mpz_cmp(t_val, buf);
    mpz_clear(buf);
    return result < 0;
}

double BigInt::to_double() const
{
    return mpz_get_d(t_val);
}

std::string BigInt::to_string() const
{
    std::string result;
    auto c_str = mpz_get_str(nullptr, 10, t_val);
    result = c_str;

    void (*freefunc)(void*, size_t);
    mp_get_memory_functions(NULL, NULL, &freefunc);
    freefunc(c_str, strlen(c_str) + 1);

    return result;
}
