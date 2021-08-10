#pragma once
#include "boost/smart_ptr/intrusive_ptr.hpp"

#include "SExprCorePoolVariant.h"
#include "SExprCoreStdVariant.h"

using SExprCoreVariant = SExprCorePoolVariant;

class SExprCore;
using SExprCoreShare = boost::intrusive_ptr<SExprCore>;


class SExprCore
{
public:

	friend void intrusive_ptr_add_ref(SExprCore* p);
	friend void intrusive_ptr_release(SExprCore* p);
	
	friend bool is_real(const SExprCore& exp);
	friend bool is_integer(const SExprCore& exp);
	friend bool is_number(const SExprCore& exp);
	friend bool is_symbol(const SExprCore& exp);
	friend bool is_atom(const SExprCore& exp);
	friend bool is_list(const SExprCore& exp);
	
	friend const Number& to_number(const SExprCore& exp);
	friend const Symbol& to_symbol(const SExprCore& exp);
	friend const DotPair& to_list(const SExprCore& exp);
	
	friend Number& to_number(SExprCore& exp);
	friend Symbol& to_symbol(SExprCore& exp);
	friend DotPair& to_list(SExprCore& exp);

	friend SExprCoreShare make_SExprCore_list_noinit();
	friend SExprCoreShare make_SExprCore_symb_noinit();
	friend SExprCoreShare make_SExprCore_numb_noinit();
public:
	SExprCore();
	~SExprCore();

	bool is_empty() const;
	void clear();
	//SExprCore(const SExprCore&) = delete;
	//SExprCore& operator=(const SExprCore&) = delete;
private:
	SExprCoreVariant t_var;
	size_t t_ref_count;
};

SExprCoreShare make_SExprCore_list_noinit();
SExprCoreShare make_SExprCore_symb_noinit();
SExprCoreShare make_SExprCore_numb_noinit();