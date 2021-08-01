#pragma once

#include "Atom.h"
#include "Number.h"

#include <list>
#include <vector>
#include <string>
#include <variant>
#include <memory>

class Cell;

class Cell {

    friend Cell make_list(std::vector<Cell>&& objs);
    friend Cell make_list(const std::list<Cell>& objs);
    friend Cell make_list(const std::vector<Cell>& objs);

    friend Cell make_number(Number&& numb);
    friend Cell make_number(const Number& numb);

    friend Cell make_symbol_cell(Symbol&& symbol);
    friend Cell make_symbol_cell(const Symbol& symbol);
    friend Cell make_symbol_cell(const std::string& symbol, Symbols& owner);
    friend Cell make_symbol_cell(std::string&& symbol, Symbols& owner);

public:
    using olist = std::vector<Cell>;
    using oatom = Atom;

private:
    using var_type = std::variant<oatom, olist>;

public:
    Cell(const Cell& rh);
    Cell(Cell&& rh) noexcept;
    ~Cell();

    Cell& operator=(const Cell& rh);

    bool is_atom() const;
    bool is_list() const;

    oatom& to_atom();
    olist& to_list();
    const oatom& to_atom()const;
    const olist& to_list()const;
private:
    //Cell() = default;
    Cell(oatom&& atom) noexcept;
    Cell(olist&& list) noexcept;

    Cell(const oatom& atom);
    Cell(const olist& list);

private:
    std::shared_ptr<var_type> t_data = nullptr;
};


inline const Atom& to_atom(const Cell& c) { return c.to_atom(); }
inline const Cell::olist& to_list(const Cell& c) { return c.to_list(); }
inline const Number& to_number(const Cell& c) { return c.to_atom().to_number(); }
inline const Symbol& to_symbol(const Cell& c) { return c.to_atom().to_symbol(); }

inline Cell::olist& to_list(Cell& c) { return c.to_list(); }


inline bool is_real_number(const Cell& c);
inline bool is_int_number(const Cell& c);

inline bool is_number(const Cell& c);
inline bool is_atom(const Cell& c);
inline bool is_symbol(const Cell& c);
inline bool is_list(const Cell& c);






inline Cell make_list(std::vector<Cell>&& objs)
{
    return Cell(std::move(objs));
}

inline Cell make_list(const std::list<Cell>& objs)
{
    return Cell(Cell::olist(begin(objs), end(objs)));
}


inline Cell make_list(const std::vector<Cell>& objs)
{
    return Cell( objs);
}

inline Cell make_number(Number&& numb)
{
    //return Cell(Atom(move(numb)));
    return Cell(Atom(numb));
}

inline Cell make_number(const Number& numb)
{
    return Cell(Atom(numb));
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
    return Cell(Atom(make_symbol(move(symbol), owner)));
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

inline bool is_atom(const Cell& c)
{
    return c.is_atom();
}

inline bool is_symbol(const Cell& c)
{
    return is_atom(c) && to_atom(c).is_symbol();
}

inline bool is_list(const Cell& c)
{
    return c.is_list();
}
