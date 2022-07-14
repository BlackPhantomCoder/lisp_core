#pragma once
#include "boost/smart_ptr/intrusive_ptr.hpp"
#include "PoolDeleter.h"

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
	enum class type {list, number, symbol, none};
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

SExprShare make_SExprShare_list_noinit();
SExprShare make_SExprShare_symb_noinit();
SExprShare make_SExprShare_numb_noinit();

//void pool_delete_dp(SExpr* ptr);
//void pool_delete_n(SExpr* ptr);
//void pool_delete_s(SExpr* ptr);