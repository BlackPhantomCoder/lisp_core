#include "Number.h"
#include <cmath>

using namespace std;

unsigned double_macheps()
{
    unsigned result = 0;
    double e = 1.0f;

    while (1.0f + e / 2.0f > 1.0f) {
        e /= 2.0f;
    }

    while (abs(e) < 1) {
        ++result;
        e *= 10;
    }
        
    return result - 1;
}

unsigned Number::epsilon = double_macheps();

Number::Number(Number&& rh):
    t_data(move(rh.t_data))
{
    rh.t_data = nullptr;
}

Number::Number(const Number& rh):
    t_data(rh.t_data)
{
}

Number::Number(double val):
    t_data(val)
{
}

Number::Number(const BigInt& val) :
    t_data(val)
{
}

Number& Number::operator=(Number&& rh)
{
    t_data = move(rh.t_data);
    rh.t_data = nullptr;
    return *this;
}

Number& Number::operator=(const Number& rh)
{
    t_data = rh.t_data;
    return *this;
}

bool Number::operator<(const Number& rh) const
{
    if (holds_alternative<double>(t_data)) {
        if (rh.is_real()) {
            return get<double>(t_data) < get<double>(rh.t_data);
        }
        else if (rh.is_integer()) {
            if (get<BigInt>(rh.t_data).is_castable_to_double()) {
                return get<double>(t_data) < get<BigInt>(rh.t_data).to_double();
            }
            else {
                return true;
            }
        }
        else {
            throw "empty number";
        }
    }
    else if (holds_alternative<BigInt>(t_data)) {
        if (rh.is_real()) {
            if (get<BigInt>(t_data).is_castable_to_double()) {
                return get<BigInt>(t_data).to_double() < get<double>(rh.t_data);
            }
            else {
                return false;
            }
        }
        else if (rh.is_integer()) {
            return get<BigInt>(t_data) < get<BigInt>(rh.t_data);
        }
        else {
            throw "empty number";
        }
    }
    else {
        throw "empty number";
    }
}

bool Number::operator>(const Number& rh) const
{
    if (holds_alternative<double>(t_data)) {
        if (rh.is_real()) {
            return get<double>(t_data) > get<double>(rh.t_data);
        }
        else if (rh.is_integer()) {
            if (get<BigInt>(rh.t_data).is_castable_to_double()) {
                return get<double>(t_data) > get<BigInt>(rh.t_data).to_double();
            }
            else {
                return false;
            }
        }
        else {
            throw "empty number";
        }
    }
    else if (holds_alternative<BigInt>(t_data)) {
        if (holds_alternative<double>(rh.t_data)) {
            if (get<BigInt>(t_data).is_castable_to_double()) {
                return get<BigInt>(t_data).to_double() > get<double>(rh.t_data);
            }
            else {
                return true;
            }
        }
        else if (rh.is_integer()) {
            return get<BigInt>(t_data) > get<BigInt>(rh.t_data);
        }
        else {
            throw "empty number";
        }
    }
    else {
        throw "empty number";
    }
}

bool Number::operator==(const Number& rh) const
{
    if (holds_alternative<double>(t_data)) {
        if (rh.is_real()) {
            return get<double>(t_data) == get<double>(rh.t_data);
        }
        else if (rh.is_integer()) {
            if (get<BigInt>(rh.t_data).is_castable_to_double()) {
                return get<double>(t_data) == get<BigInt>(rh.t_data).to_double();
            }
            else {
                return false;
            }
        }
        else {
            throw "empty number";
        }
    }
    else if (holds_alternative<BigInt>(t_data)) {
        if (rh.is_real()) {
            if (get<BigInt>(t_data).is_castable_to_double()) {
                return get<BigInt>(t_data).to_double() == get<double>(rh.t_data);
            }
            else {
                return false;
            }
        }
        else if (rh.is_integer()) {
            return get<BigInt>(t_data) == get<BigInt>(rh.t_data);
        }
        else {
            throw "empty number";
        }
    }
    else {
        throw "empty number";
    }
}

bool Number::operator<=(const Number& rh) const
{
    if (holds_alternative<double>(t_data)) {
        if (rh.is_real()) {
            return get<double>(t_data) <= get<double>(rh.t_data);
        }
        else if (rh.is_integer()) {
            if (get<BigInt>(rh.t_data).is_castable_to_double()) {
                return get<double>(t_data) <= get<BigInt>(rh.t_data).to_double();
            }
            else {
                return true;
            }
        }
        else {
            throw "empty number";
        }
    }
    else if (holds_alternative<BigInt>(t_data)) {
        if (rh.is_real()) {
            if (get<BigInt>(t_data).is_castable_to_double()) {
                return get<BigInt>(t_data).to_double() <= get<double>(rh.t_data);
            }
            else {
                return false;
            }
        }
        else if (rh.is_integer()) {
            return get<BigInt>(t_data) <= get<BigInt>(rh.t_data);
        }
        else {
            throw "empty number";
        }
    }
    else {
        throw "empty number";
    }
}

bool Number::operator>=(const Number& rh) const
{
    if (holds_alternative<double>(t_data)) {
        if (rh.to_real()) {
            return get<double>(t_data) >= get<double>(rh.t_data);
        }
        else if (rh.is_integer()) {
            if (get<BigInt>(rh.t_data).is_castable_to_double()) {
                return get<double>(t_data) >= get<BigInt>(rh.t_data).to_double();
            }
            else {
                return false;
            }
        }
        else {
            throw "empty number";
        }
    }
    else if (holds_alternative<BigInt>(t_data)) {
        if (rh.is_real()) {
            if (get<BigInt>(t_data).is_castable_to_double()) {
                return get<BigInt>(t_data).to_double() >= get<double>(rh.t_data);
            }
            else {
                return false;
            }
        }
        else if (rh.is_integer()) {
            return get<BigInt>(t_data) >= get<BigInt>(rh.t_data);
        }
        else {
            throw "empty number";
        }
    }
    else {
        throw "empty number";
    }
}

// point must be Number::real
void sum_double(std::variant<Number::integer, Number::real, void*>& point, double rh) {
    double x = get<double>(point);

    int exponent1 = 0;
    int exponent2 = 0;
    int exponent3 = 0;

    double significand1 = frexp(x, &exponent1);
    double significand2 = frexp(rh, &exponent2);
    double significand3 = frexp(std::numeric_limits<double>::max(), &exponent3);

    if (exponent1 + exponent2 >= exponent3 || (exponent1 + exponent2 == exponent3 && (significand1 + significand2 >= significand3))) {
        point = BigInt(get<double>(point));
        get<BigInt>(point) += BigInt(rh);
    }
    else {
        get<double>(point) += rh;
    }
}

// point must be Number::real
void mul_double(std::variant<Number::integer, Number::real, void*>& point, double rh) {
    double x = get<double>(point);

    int exponent1 = 0;
    int exponent2 = 0;
    int exponent3 = 0;

    double significand1 = frexp(x, &exponent1);
    double significand2 = frexp(rh, &exponent2);
    double significand3 = frexp(std::numeric_limits<double>::max(), &exponent3);

    if (exponent1 + exponent2 >= exponent3) {
        point = BigInt(get<double>(point));
        get<BigInt>(point) *= BigInt(rh);
    }
    else {
        get<double>(point) *= rh;
    }
}

// point must be Number::real
void del_double(std::variant<Number::integer, Number::real, void*>& point, double rh) {
    double x = get<double>(point);

    int exponent1 = 0;
    int exponent2 = 0;
    int exponent3 = 0;

    double significand1 = frexp(x, &exponent1);
    double significand2 = frexp(rh, &exponent2);
    double significand3 = frexp(std::numeric_limits<double>::max(), &exponent3);

    if (std::abs(exponent1 - exponent2) >= exponent3) {
        point = BigInt(get<double>(point));
        get<BigInt>(point).div(BigInt(rh));
    }
    else {
        get<double>(point) /= rh;
    }
}

Number& Number::operator+=(const Number& rh)
{
    if (holds_alternative<double>(t_data)) {
        if (rh.is_real()) {
            sum_double(t_data, get<double>(rh.t_data));
        }
        else if (rh.is_integer()) {
            if (get<BigInt>(rh.t_data).is_castable_to_double()) {
                sum_double(t_data, get<BigInt>(rh.t_data).to_double());
            }
            else {
                t_data = BigInt(get<double>(t_data));
                get<BigInt>(t_data) += get<BigInt>(rh.t_data);
            }
        }
        else {
            throw "empty number";
        }
    }
    else if (holds_alternative<BigInt>(t_data)) {
        if (rh.is_real()) {
            auto result = rh;
            result += *this;
            t_data = result.t_data;
        }
        else if (rh.is_integer()) {
            get<BigInt>(t_data) += get<BigInt>(rh.t_data);
        }
        else {
            throw "empty number";
        }
    }
    else {
        throw "empty number";
    }
    return *this;
}

Number& Number::operator-=(const Number& rh)
{
    if (holds_alternative<double>(t_data)) {
        if (rh.is_real()) {
            sum_double(t_data, -get<double>(rh.t_data));
        }
        else if (rh.is_integer()) {
            if (get<BigInt>(rh.t_data).is_castable_to_double()) {
                sum_double(t_data, -get<BigInt>(rh.t_data).to_double());
            }
            else {
                t_data = BigInt(get<double>(t_data));
                get<BigInt>(t_data) -= get<BigInt>(rh.t_data);
            }
        }
        else {
            throw "empty number";
        }
    }
    else if (holds_alternative<BigInt>(t_data)) {
        if (rh.is_real()) {
            auto result = rh;
            result.minus();
            result += *this;
            t_data = result.t_data;
        }
        else if (rh.is_integer()) {
            get<BigInt>(t_data) -= get<BigInt>(rh.t_data);
        }
        else {
            throw "empty number";
        }
    }
    else {
        throw "empty number";
    }
    return *this;
}

Number& Number::operator*=(const Number& rh)
{
    if (holds_alternative<double>(t_data)) {
        if (rh.is_real()) {
            mul_double(t_data, get<double>(rh.t_data));
        }
        else if (rh.is_integer()) {
            if (get<BigInt>(rh.t_data).is_castable_to_double()) {
                mul_double(t_data, get<BigInt>(rh.t_data).to_double());
            }
            else {
                t_data = BigInt(get<double>(t_data));
                get<BigInt>(t_data) *= get<BigInt>(rh.t_data);
            }
        }
        else {
            throw "empty number";
        }
    }
    else if (holds_alternative<BigInt>(t_data)) {
        if (rh.is_real()) {
            auto result = rh;
            result *= *this;
            t_data = result.t_data;
        }
        else if (rh.is_integer()) {
            get<BigInt>(t_data) *= get<BigInt>(rh.t_data);
        }
        else {
            throw "empty number";
        }
    }
    else {
        throw "empty number";
    }
    return *this;
}

Number& Number::operator/=(const Number& rh)
{
    if (holds_alternative<double>(t_data)) {
        if (rh.is_real()) {
            del_double(t_data, get<double>(rh.t_data));
        }
        else if (rh.is_integer()) {
            if (get<BigInt>(rh.t_data).is_castable_to_double()) {
                del_double(t_data, get<BigInt>(rh.t_data).to_double());
            }
            else {
                t_data = BigInt(get<double>(t_data));
                get<BigInt>(t_data).div(get<BigInt>(rh.t_data));
            }
        }
        else {
            throw "empty number";
        }
    }
    else if (holds_alternative<BigInt>(t_data)) {
        if (rh.is_real()) {
            if (get<BigInt>(t_data).is_castable_to_double()) {
                double x = get<BigInt>(t_data).to_double();
                double val = get<double>(rh.t_data);

                int exponent1 = 0;
                int exponent2 = 0;
                int exponent3 = 0;

                double significand1 = frexp(x, &exponent1);
                double significand2 = frexp(val, &exponent2);
                double significand3 = frexp(std::numeric_limits<double>::max(), &exponent3);

                if (std::abs(exponent1 - exponent2) >= exponent3) {
                    get<BigInt>(t_data).div(BigInt(get<double>(rh.t_data)));
                }
                else {
                    t_data = get<BigInt>(t_data).to_double();
                    get<double>(t_data) /= val;
                }
            }
            else {
                get<BigInt>(t_data).div(BigInt(get<double>(rh.t_data)));
            }
        }
        else if (rh.is_integer()) {
            if (
                    get<BigInt>(t_data).is_castable_to_double()
                    &&
                    get<BigInt>(rh.t_data).is_castable_to_double()
                    &&
                    (!(get<BigInt>(t_data).mod(get<BigInt>(rh.t_data)) == BigInt(0l)))
                ) {
                double x = get<BigInt>(t_data).to_double();
                double val = get<BigInt>(rh.t_data).to_double();

                int exponent1 = 0;
                int exponent2 = 0;
                int exponent3 = 0;

                double significand1 = frexp(x, &exponent1);
                double significand2 = frexp(val, &exponent2);
                double significand3 = frexp(std::numeric_limits<double>::max(), &exponent3);

                if (std::abs(exponent1 - exponent2) >= exponent3) {
                    get<BigInt>(t_data).div(get<BigInt>(rh.t_data));
                }
                else {
                    t_data = get<BigInt>(t_data).to_double();
                    get<double>(t_data) /= val;
                }
            }
            else {
                get<BigInt>(t_data).div(get<BigInt>(rh.t_data));
            }
        }
        else {
            throw "empty number";
        }
    }
    else {
        throw "empty number";
    }
    return *this;
}

Number& Number::div(const Number& rh)
{
    *this /= rh;
    if (holds_alternative<double>(t_data)) {
        t_data = BigInt(get<double>(t_data));
    }
    else if (!holds_alternative<BigInt>(t_data)) {
        throw "empty number";
    }
    return *this;
}

Number Number::mod(const Number& rh)const
{
    auto result = *this;
    result /= rh;
    if (result.is_integer()) {
        return Number(BigInt(0l));
    }
    else if (result.is_real()) {
        double a = 0;
        double b = modf(result.to_real(), &a);
        double result2 = 0;
        unsigned i = 0;
        while (b > 0 && i != (Number::epsilon - 1)) {
            b *= 10;
            b = modf(b, &a);
            result2 += a;
        }
        return Number(result2);
    }
    else {
        throw "empty number";
    }
    return *this;
}

void Number::minus()
{
    if (holds_alternative<BigInt>(t_data)) {
        get<BigInt>(t_data).minus();
    }
    else if (holds_alternative<double>(t_data)) {
        get<double>(t_data) *= -1;
    }
    else {
        throw "empty number";
    }
}

void Number::abs()
{
    if (holds_alternative<BigInt>(t_data)) {
        get<BigInt>(t_data).abs();
    }
    else if(holds_alternative<double>(t_data)) {
        get<double>(t_data) = std::abs(get<double>(t_data));
    }
    else {
        throw "empty number";
    }
}

bool Number::is_integer() const
{
    return holds_alternative<BigInt>(t_data);
}

bool Number::is_real() const
{
    return holds_alternative<double>(t_data);
}

const Number::integer& Number::to_integer() const
{
    return get<BigInt>(t_data);
}

const Number::real& Number::to_real() const
{
    return get<double>(t_data);
}

std::string Number::to_string() const
{
    if (holds_alternative<double>(t_data)) {
        return std::to_string(get<double>(t_data));
    }
    else {
        return get<BigInt>(t_data).to_string();
    }
}
