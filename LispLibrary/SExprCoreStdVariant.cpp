#include "SExprCoreStdVariant.h"

#include "Number.h"
#include "DotPair.h"
#include "Symbol.h"

using namespace std;

SExprCoreStdVariant::~SExprCoreStdVariant()
{
    clear();
}

SExprCoreStdVariant::SExprCoreStdVariant(SExprCoreStdVariant&& rh) noexcept :
    t_data(move(rh.t_data))
{
    rh.t_data = monostate{};
}

SExprCoreStdVariant& SExprCoreStdVariant::operator=(SExprCoreStdVariant&& rh) noexcept
{
    if (this != &rh) {
        t_data = rh.t_data;
        rh.t_data = monostate{};
    }
    return *this;
}

bool SExprCoreStdVariant::is_empty() const
{
    return holds_alternative<monostate>(t_data);
}

void SExprCoreStdVariant::clear()
{
    t_data = monostate{};
}

bool is_number(const SExprCoreStdVariant& exp)
{
    return holds_alternative<shared_ptr<Number>>(exp.t_data);
}

bool is_symbol(const SExprCoreStdVariant& exp)
{
    return holds_alternative<shared_ptr<Symbol>>(exp.t_data);
}

bool is_list(const SExprCoreStdVariant& exp)
{
    return holds_alternative<shared_ptr<DotPair>>(exp.t_data);
}

Number& to_number( SExprCoreStdVariant& exp)
{
    return *get< shared_ptr<Number>>(exp.t_data);
}

Symbol& to_symbol( SExprCoreStdVariant& exp)
{
    return *get<shared_ptr<Symbol>>(exp.t_data);
}

DotPair& to_list( SExprCoreStdVariant& exp)
{
    return *get<shared_ptr<DotPair>>(exp.t_data);
}

const Number& to_number(const SExprCoreStdVariant& exp)
{
    return *get<shared_ptr<Number>>(exp.t_data);
}

const Symbol& to_symbol(const SExprCoreStdVariant& exp)
{
    return *get<shared_ptr<Symbol>>(exp.t_data);
}

const DotPair& to_list(const SExprCoreStdVariant& exp)
{
    return *get<shared_ptr<DotPair>>(exp.t_data);
}

void make_SExprCoreVariant_list_noinit(SExprCoreStdVariant& var)
{
    var.t_data = make_shared<DotPair>();
}

void make_SExprCoreVariant_symb_noinit(SExprCoreStdVariant& var)
{
    var.t_data = make_shared<Symbol>();
}

void make_SExprCoreVariant_numb_noinit(SExprCoreStdVariant& var)
{
    var.t_data = make_shared<Number>();
}
