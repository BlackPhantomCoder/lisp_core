#pragma once
#include "boost/smart_ptr/intrusive_ptr.hpp"

class Number;
class Symbol;
class DotPair;

class SExpr;
using SExprShare = boost::intrusive_ptr<SExpr>;

class SExpr
{
	friend void intrusive_ptr_add_ref(SExpr* p);
	friend void intrusive_ptr_release(SExpr* p);
public:
	//????? ???????
	enum class type {list = 0, number = 1, symbol = 2, none};
public:
	SExpr(type type);
	SExpr(SExpr&& rh) noexcept;
	SExpr(const SExpr& rh);

	SExpr& operator=(SExpr&& rh) noexcept;
	SExpr& operator=(const SExpr& rh);

	~SExpr() = default;

	bool is_real() const;
	bool is_integer() const;
	bool is_number() const;
	bool is_symbol() const;
	bool is_atom() const;
	bool is_list() const;

	const Number& to_number() const;
	const Symbol& to_symbol() const;
	const DotPair& to_list() const;
	
	Number& to_number();
	Symbol& to_symbol();
	DotPair& to_list();
private:
	size_t t_count = 0;
	type t_type;
};

