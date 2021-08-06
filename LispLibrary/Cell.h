#pragma once

#include "Atom.h"
#include "Number.h"
#include <vector>
#include <string>
#include <variant>
#include <memory>

class DotPair;
class SymbolsProvider;

class Cell {

    friend inline Cell deep_copy_list_cell(const DotPair& lst);
    friend inline Cell make_list_cell(std::initializer_list<Cell> l, SymbolsProvider& provider);
    template< class InputIt >
    friend Cell make_list_cell(InputIt first, InputIt last, SymbolsProvider& provider);
    friend Cell make_list_cell(DotPair&& objs);
    friend Cell make_list_cell(const DotPair& objs);
    friend inline Cell cons_cell(const Cell& f, const Cell& s, SymbolsProvider& provider);
    friend inline Cell make_empty_list_cell(SymbolsProvider& provider);

    friend Cell make_atom(Atom&& atom);
    friend Cell make_atom(const Atom& atom);

    friend Cell make_number(Number&& numb);
    friend Cell make_number(const Number& numb);

    friend Cell make_symbol_cell(Atom&& symbol);
    friend Cell make_symbol_cell(const Atom& symbol);
    friend Cell make_symbol_cell(Symbol&& symbol);
    friend Cell make_symbol_cell(const Symbol& symbol);
    friend Cell make_symbol_cell(const std::string& symbol, Symbols& owner);
    friend Cell make_symbol_cell(std::string&& symbol, Symbols& owner);

//private:
    using var_type = std::variant<Atom, DotPair>;

public:
    Cell(const Cell& rh);
    Cell(Cell&& rh) noexcept;
    ~Cell() = default;

    Cell& operator=(const Cell& rh);

    bool is_atom() const;
    bool is_list() const;

    Atom& to_atom();
    DotPair& to_list();
    const Atom& to_atom()const;
    const DotPair& to_list()const;
private:
    Cell(Atom&& atom) noexcept;
    Cell(DotPair&& list) noexcept;

    Cell(const Atom& atom);
    Cell(const DotPair& list);

private:
    std::shared_ptr<var_type> t_data = nullptr;
};

inline Cell deep_copy_list_cell(const DotPair& lst);
inline Cell make_list_cell(std::initializer_list<Cell> l, SymbolsProvider& provider);
template< class InputIt >
Cell make_list_cell(InputIt first, InputIt last, SymbolsProvider& provider);
Cell make_list_cell(DotPair&& objs);
Cell make_list_cell(const DotPair& objs);

inline const Atom& to_atom(const Cell& c) { return c.to_atom(); }
inline const Number& to_number(const Cell& c) { return c.to_atom().to_number(); }
inline const Symbol& to_symbol(const Cell& c) { return c.to_atom().to_symbol(); }
inline const Number& to_number(const Atom& c) { return c.to_number(); }
inline const Symbol& to_symbol(const Atom& c) { return c.to_symbol(); }

inline bool is_real_number(const Cell& c);
inline bool is_int_number(const Cell& c);

inline bool is_atom(const Cell& c);
inline bool is_list(const Cell& c);

inline bool is_number(const Cell& c);
inline bool is_number(const Atom& c);
inline bool is_symbol(const Cell& c);
inline bool is_symbol(const Atom& c);