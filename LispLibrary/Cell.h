#pragma once

#include "Atom.h"
#include "Number.h"

#include <list>
#include <vector>
#include <string>
#include <variant>
#include <memory>

class Cell;
using DPair = std::vector<Cell>;

class Cell {

    friend Cell make_list(std::vector<Cell>&& objs);
    friend Cell make_list(const std::list<Cell>& objs);
    friend Cell make_list(const std::vector<Cell>& objs);

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
    using var_type = std::variant<Atom, DPair>;

public:
    Cell(const Cell& rh);
    Cell(Cell&& rh) noexcept;
    ~Cell() = default;

    Cell& operator=(const Cell& rh);

    bool is_atom() const;
    bool is_list() const;

    Atom& to_atom();
    DPair& to_list();
    const Atom& to_atom()const;
    const DPair& to_list()const;
private:
    //Cell() = default;
    Cell(Atom&& atom) noexcept;
    Cell(DPair&& list) noexcept;

    Cell(const Atom& atom);
    Cell(const DPair& list);

private:
    std::shared_ptr<var_type> t_data = nullptr;
};


inline const Atom& to_atom(const Cell& c) { return c.to_atom(); }
inline const DPair& to_list(const Cell& c) { return c.to_list(); }
inline const Number& to_number(const Cell& c) { return c.to_atom().to_number(); }
inline const Symbol& to_symbol(const Cell& c) { return c.to_atom().to_symbol(); }
inline const Number& to_number(const Atom& c) { return c.to_number(); }
inline const Symbol& to_symbol(const Atom& c) { return c.to_symbol(); }

inline DPair& to_list(Cell& c) { return c.to_list(); }


inline bool is_real_number(const Cell& c);
inline bool is_int_number(const Cell& c);


inline bool is_atom(const Cell& c);
inline bool is_list(const Cell& c);

inline bool is_number(const Cell& c);
inline bool is_number(const Atom& c);
inline bool is_symbol(const Cell& c);
inline bool is_symbol(const Atom& c);






inline Cell make_list(std::vector<Cell>&& objs)
{
    return Cell(std::move(objs));
}

inline Cell make_list(const std::list<Cell>& objs)
{
    return Cell(DPair(begin(objs), end(objs)));
}


inline Cell make_list(const std::vector<Cell>& objs)
{
    return Cell( objs);
}

inline Cell make_number(Number&& numb)
{
    return Cell(Atom(std::move(numb)));
}

inline Cell make_number(const Number& numb)
{
    return Cell(Atom(numb));
}

inline Cell make_symbol_cell(Atom&& symbol) {
    return Cell(std::move(symbol));
}

inline Cell make_symbol_cell(const Atom& symbol) {
    return Cell(symbol);
}

inline Cell make_symbol_cell(Symbol&& symbol)
{
    return Cell(Atom(std::move(symbol)));
}

inline Cell make_symbol_cell(const Symbol& symbol)
{
    return Cell(Atom(Symbol(symbol)));
}

inline Cell make_symbol_cell(std::string&& symbol, Symbols& owner)
{
    return Cell(Atom(make_symbol(std:: move(symbol), owner)));
}

inline Cell make_symbol_cell(const std::string& symbol, Symbols& owner)
{
    return Cell(Atom(make_symbol(symbol, owner)));
}

inline bool is_real_number(const Cell& c)
{
    return is_number(c)  && (to_number(c)).is_real();
}

inline bool is_int_number(const Cell& c)
{
    return is_number(c) && (to_number(c)).is_integer();
}

inline bool is_number(const Cell& c)
{
    return is_atom(c) && to_atom(c).is_number();
}

inline bool is_number(const Atom& c)
{
    return c.is_number();
}

inline bool is_atom(const Cell& c)
{
    return c.is_atom();
}

inline bool is_symbol(const Cell& c)
{
    return is_atom(c) && to_atom(c).is_symbol();
}

inline bool is_symbol(const Atom& c)
{
    return c.is_symbol();
}

inline bool is_list(const Cell& c)
{
    return c.is_list();
}

inline Cell make_atom(Atom&& atom) {
    return Cell(std::move(atom));
}

inline Cell make_atom(const Atom& atom) {
    return Cell(atom);
}