#pragma once
#include <variant>

class DotPair;
class Symbol;
class Number;


struct SExprCorePoolVariant {
	SExprCorePoolVariant() = default;
	SExprCorePoolVariant(SExprCorePoolVariant&& rh) noexcept;
	SExprCorePoolVariant& operator=(SExprCorePoolVariant&& rh) noexcept;

	SExprCorePoolVariant(const SExprCorePoolVariant& rh) = default;
	SExprCorePoolVariant& operator=(const SExprCorePoolVariant& rh) = default;

	std::variant< DotPair*, Symbol*, Number*, std::monostate> t_data = std::monostate{};

	bool is_empty() const;
	void clear();

	~SExprCorePoolVariant();
};

bool is_number(const SExprCorePoolVariant& exp);
bool is_symbol(const SExprCorePoolVariant& exp);
bool is_list(const SExprCorePoolVariant& exp);

const Number& to_number(const SExprCorePoolVariant& exp);
const Symbol& to_symbol(const SExprCorePoolVariant& exp);
const DotPair& to_list(const SExprCorePoolVariant& exp);

Number& to_number(SExprCorePoolVariant& exp);
Symbol& to_symbol(SExprCorePoolVariant& exp);
DotPair& to_list(SExprCorePoolVariant& exp);

void make_SExprCoreVariant_list_noinit(SExprCorePoolVariant& var);
void make_SExprCoreVariant_symb_noinit(SExprCorePoolVariant& var);
void make_SExprCoreVariant_numb_noinit(SExprCorePoolVariant& var);
