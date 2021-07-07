#pragma once
#include <list>
#include <vector>
#include <string>

// return true iff given character is '0'..'9'
inline bool isdig(char c);
inline bool is_number(const char* str);
inline bool is_symbol(const char* str);


struct cell {
    using olist = std::vector<cell>;
    using oatom = std::string;


    cell() = default;
    cell(cell&& rh):
        t_atom(std::move(rh.t_atom)),
        t_list(std::move(rh.t_list))
    {
    }

    cell(const cell& rh) :
        t_atom(rh.t_atom),
        t_list(rh.t_list)
    {
    }

    cell(oatom&& atom):
        t_atom(std::move(atom)),
        t_list()
    {
    }

    cell(olist&& list) : 
        t_atom(),
        t_list(std::move(list))
    {
    }

    ~cell() {
        clear();
    }

    cell& operator=(cell&& rh) {
        if (&rh == this) return *this;
        clear();
        t_list = std::move(rh.t_list);
        t_atom = std::move(rh.t_atom);
        return *this;
    }

    cell& operator=(const cell& rh) = default;

    bool is_atom() const { return !t_atom.empty(); };
    bool is_list() const { return !is_atom(); };

    oatom& to_atom() {if(!is_atom())throw "to_atom"; return t_atom; };
    olist& to_list() {if (!is_list())throw "to_list"; return t_list; };

    const oatom& to_atom()const {if (!is_atom())throw "to_atom"; return t_atom;};
    const olist& to_list()const {if (!is_list())throw "to_list"; return t_list; };
private:
    void clear() {
        t_atom.clear();
        t_list.clear();
    }
private:
    oatom t_atom;
    olist t_list;
};









inline bool isdig(char c)
{
    return isdigit(static_cast<unsigned char>(c)) != 0;
}

inline bool is_number(const char* str)
{
    return !(!str || str[0] == '\0') && (isdig(str[0u]) || (str[0u] == '-' && isdig(str[1u])));
}

inline bool is_symbol(const char* str){
    return !is_number(str);
}

inline cell make_list(std::vector<cell>&& objs)
{
    return cell( cell::olist(std::move(objs)));
}

inline cell make_list(const std::list<cell>& objs)
{
    return cell(cell::olist(begin(objs), end(objs)));
}


inline cell make_list(const std::vector<cell>& objs)
{
    return cell( cell::olist(objs));
}

inline cell make_atom(const std::string& symb)
{
    return cell( cell::oatom(symb));
}

inline cell make_atom(std::string&& symb)
{
    return cell( cell::oatom(std::move(symb)));
}
