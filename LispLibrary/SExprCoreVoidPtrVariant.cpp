#include "SExprCorePoolVariant.h"
#include "CoreData.h"

#include "Number.h"
#include "DotPair.h"
#include "Symbol.h"

using namespace std;

static CoreData::ObjPoll<Number> numb_pool;
static CoreData::ObjPoll<Symbol> symbol_pool;
static CoreData::ObjPoll<DotPair> dpair_pool;

bool is_number(const SExprCorePoolVariant& exp)
{
	return holds_alternative<Number*>(exp.t_data);
}

bool is_symbol(const SExprCorePoolVariant& exp)
{
	return holds_alternative<Symbol*>(exp.t_data);
}


bool is_list(const SExprCorePoolVariant& exp)
{
	return holds_alternative<DotPair*>(exp.t_data);
}

Number& to_number(SExprCorePoolVariant& exp)
{
	return *get<Number*>(exp.t_data);
}

Symbol& to_symbol(SExprCorePoolVariant& exp)
{
	return *get<Symbol*>(exp.t_data);
}

DotPair& to_list(SExprCorePoolVariant& exp)
{
	return *get<DotPair*>(exp.t_data);
}

void make_SExprCoreVariant_list_noinit(SExprCorePoolVariant& var)
{
	var.t_data = dpair_pool.get_default();
}

void make_SExprCoreVariant_symb_noinit(SExprCorePoolVariant& var)
{
	var.t_data = symbol_pool.get_default();
}

void make_SExprCoreVariant_numb_noinit(SExprCorePoolVariant& var)
{
	var.t_data = numb_pool.get_default();
}

const Symbol& to_symbol(const SExprCorePoolVariant& exp)
{
	return *get<Symbol*>(exp.t_data);
}

const DotPair& to_list(const SExprCorePoolVariant& exp)
{
	return *get<DotPair*>(exp.t_data);
}

const Number& to_number(const SExprCorePoolVariant& exp)
{
	return *get<Number*>(exp.t_data);
}

SExprCorePoolVariant::SExprCorePoolVariant(SExprCorePoolVariant&& rh) noexcept:
	t_data(move(rh.t_data))
{
	rh.t_data = monostate{};
}

SExprCorePoolVariant& SExprCorePoolVariant::operator=(SExprCorePoolVariant&& rh) noexcept
{
	if (this != &rh) {
		t_data = move(rh.t_data);
		rh.t_data = monostate{};
	}
	return *this;
}

bool SExprCorePoolVariant::is_empty() const
{
	return holds_alternative<monostate>(t_data);
}

void SExprCorePoolVariant::clear()
{
	if (!is_empty()) {

		if (is_list(*this)) {
			get<DotPair*>(t_data)->clear();
			dpair_pool.free(get<DotPair*>(t_data));
		}
		else if (is_symbol(*this)) {
			get<Symbol*>(t_data)->clear();
			symbol_pool.free(get<Symbol*>(t_data));
		}
		else if (is_number(*this)) {
			get<Number*>(t_data)->clear();
			numb_pool.free(get<Number*>(t_data));
		}
		else {
			throw "unknown type";
		}

		t_data = monostate{};
	}
}

SExprCorePoolVariant::~SExprCorePoolVariant()
{
	clear();
}
