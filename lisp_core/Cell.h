#pragma once

#include "Atom.h"

#include <list>
#include <vector>
#include <string>
#include <variant>

class Cell;

inline Cell make_list(std::vector<Cell>&& objs);
inline Cell make_list(const std::list<Cell>& objs);
inline Cell make_list(const std::vector<Cell>& objs);

inline Cell make_atom(Atom&& atom);
inline Cell make_atom(const Atom& atom);


class Cell {
public:
    using olist = std::vector<Cell>;
    using oatom = Atom;

public:
    Cell() = default;
    Cell(Cell&& rh) noexcept;
    Cell(oatom&& atom) noexcept;
    Cell(olist&& list) noexcept;

    Cell(const oatom& atom);
    Cell(const olist& list);
    Cell(const Cell& rh);
    ~Cell();

    Cell& operator=(Cell&& rh)noexcept;
    Cell& operator=(const Cell& rh);

    bool is_atom() const;
    bool is_list() const;

    oatom& to_atom();
    olist& to_list();
    const oatom& to_atom()const;
    const olist& to_list()const;
private:
    std::variant<oatom, olist, void*> t_data = nullptr;
};








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

inline Cell make_atom(Atom&& atom)
{
    return Cell(std::move(atom));
}

inline Cell make_atom(const Atom& atom)
{
    return Cell(atom);
}