#include "SExprCore.h"
#include "MPool.h"
#include "Number.h"
#include "DotPair.h"
#include "Symbol.h"
#include "CoreData.h"

using namespace std;

static CoreData::ObjPoll<SExprCore> pool;


void intrusive_ptr_add_ref(SExprCore* p)
{
	++p->t_ref_count;
}

void intrusive_ptr_release(SExprCore* p)
{
	--p->t_ref_count;
	if (p->t_ref_count == 0) {
		p->clear();
		pool.free(p);
	}
}

bool is_real(const SExprCore& exp)
{
	return is_number(exp) && is_real(to_number(exp));
}

bool is_integer(const SExprCore& exp)
{
	return is_number(exp) && is_integer(to_number(exp));
}


bool is_number(const SExprCore& exp)
{
	return is_number(exp.t_var);
}

bool is_symbol(const SExprCore& exp)
{
	return is_symbol(exp.t_var);
}

bool is_atom(const SExprCore& exp)
{
	return is_symbol(exp) || is_number(exp);;
}

bool is_list(const SExprCore& exp)
{
	return is_list(exp.t_var);
}

Number& to_number(SExprCore& exp)
{
	return to_number(exp.t_var);
}

Symbol& to_symbol(SExprCore& exp)
{
	return to_symbol(exp.t_var);
}

DotPair& to_list(SExprCore& exp)
{
	return to_list(exp.t_var);
}

const Symbol& to_symbol(const SExprCore& exp)
{
	return to_symbol(exp.t_var);
}

const DotPair& to_list(const SExprCore& exp)
{
	return to_list(exp.t_var);
}

const Number& to_number(const SExprCore& exp)
{
	return to_number(exp.t_var);
}

SExprCoreShare make_SExprCore_list_noinit()
{
	SExprCoreShare exp = pool.get_default();
	make_SExprCoreVariant_list_noinit(exp->t_var);
	return exp;
}

SExprCoreShare make_SExprCore_symb_noinit()
{
	SExprCoreShare exp = pool.get_default();
	make_SExprCoreVariant_symb_noinit(exp->t_var);
	return exp;
}

SExprCoreShare make_SExprCore_numb_noinit()
{
	SExprCoreShare exp = pool.get_default();
	make_SExprCoreVariant_numb_noinit(exp->t_var);
	return exp;
}

SExprCore::SExprCore():
	t_ref_count(0)
{
}

SExprCore::~SExprCore()
{
	if (t_ref_count) {
		std::cerr << "SExprCore: refcount > 0" << endl;
	}
	t_var.clear();
}

bool SExprCore::is_empty() const
{
	return t_var.is_empty();
}

void SExprCore::clear()
{
	if (t_ref_count) {
		throw "clear of shared sexpr";
	}
	t_var.clear();
}
