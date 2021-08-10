#pragma once

#include <variant>
#include <memory>

class DotPair;
class Symbol;
class Number;


struct SExprCoreStdVariant {
	SExprCoreStdVariant() = default;
	~SExprCoreStdVariant();
	SExprCoreStdVariant(SExprCoreStdVariant&& rh) noexcept;
	SExprCoreStdVariant& operator=(SExprCoreStdVariant&& rh) noexcept;

	SExprCoreStdVariant(const SExprCoreStdVariant& rh) = default;
	SExprCoreStdVariant& operator=(const SExprCoreStdVariant& rh) = default;
	std::variant<std::shared_ptr<DotPair>, std::shared_ptr<Symbol>, std::shared_ptr<Number>, std::monostate> t_data
		= std::monostate{};

	bool is_empty() const;
	void clear();
};

bool is_number(const SExprCoreStdVariant& exp);
bool is_symbol(const SExprCoreStdVariant& exp);
bool is_list(const SExprCoreStdVariant& exp);

const Number& to_number(const SExprCoreStdVariant& exp);
const Symbol& to_symbol(const SExprCoreStdVariant& exp);
const DotPair& to_list(const SExprCoreStdVariant& exp);

Number& to_number(SExprCoreStdVariant& exp);
Symbol& to_symbol(SExprCoreStdVariant& exp);
DotPair& to_list(SExprCoreStdVariant& exp);

void make_SExprCoreVariant_list_noinit(SExprCoreStdVariant& var);
void make_SExprCoreVariant_symb_noinit(SExprCoreStdVariant& var);
void make_SExprCoreVariant_numb_noinit(SExprCoreStdVariant& var);