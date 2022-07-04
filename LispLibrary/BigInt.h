#pragma once

#include "gmp/include/gmp.h"
#include <string>

class BigInt
{
public:
	BigInt(const BigInt& rh);
	explicit BigInt(const std::string& str);
	explicit BigInt(long n);

	BigInt(double n);

	~BigInt();

	BigInt& operator=(const BigInt& rh);

	bool operator<(const BigInt& rh)const;
	bool operator>(const BigInt& rh)const;
	bool operator==(const BigInt& rh)const;

	bool operator<=(const BigInt& rh)const;
	bool operator>=(const BigInt& rh)const;

	BigInt& operator+=(const BigInt& rh);
	BigInt& operator-=(const BigInt& rh);
	BigInt& operator*=(const BigInt& rh);
	BigInt& div(const BigInt& rh);
	BigInt mod(const BigInt& rh) const;

	void minus();
	void abs();

	bool is_castable_to_double()const;
	double to_double()const;
	int to_int() const;
	std::string to_string()const;
private:
	mpz_t t_val;
};