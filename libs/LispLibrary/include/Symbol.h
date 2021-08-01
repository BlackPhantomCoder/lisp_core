#pragma once
#include <string>
#include <iostream>
#include <deque>
#include <unordered_map>
#include <list>

class Symbols {
public:
    using oblist = std::list<std::string_view>;
    using symb_mp = std::unordered_map<std::string, std::pair<oblist::iterator, size_t>>;

    class symbol_core {
    public:
        symbol_core(symb_mp::iterator it, Symbols& owner);
        symbol_core(const symbol_core& rh);
        ~symbol_core();
        const char* str() const;
        size_t size() const;
        bool operator==(const symbol_core& rh) const;

    private:
        symb_mp::iterator t_base;
        std::reference_wrapper< Symbols> t_owner;
    };
public:

    Symbols::symbol_core make_or_copy(std::string&& str);
    Symbols::symbol_core make_or_copy(const std::string& str);
    void del_symbol(symb_mp::iterator);

    const oblist& get_lst() const;
private:
    symb_mp t_symbol_to_it_and_count;
    oblist t_oblist;
};


class Symbol {
    friend Symbol make_symbol(const std::string& data, Symbols& owner);
    friend Symbol make_symbol(std::string&& data, Symbols& owner);
public:
    Symbol(const Symbol& rh);
    Symbol& operator=(const Symbol& rh);

    bool operator<(const Symbol& symb)const;
    bool operator==(const Symbol& symb)const;
    bool operator!=(const Symbol& symb)const;

    size_t size()const;
    const char* to_string()const;
private:
    Symbol(const Symbols::symbol_core& data);
private:
    Symbols::symbol_core t_data;
};



Symbol make_symbol(const std::string& data, Symbols& owner);
Symbol make_symbol(std::string&& data, Symbols& owner);

namespace std
{
    template<> struct hash<Symbol>
    {
        std::size_t operator()(Symbol const& s) const noexcept
        {
            return std::hash<const char*>{}(s.to_string());
        }
    };
}

bool operator==(const Symbol& lh, const std::string& rh);
bool operator==(const std::string& lh, const Symbol& rh);

bool operator!=(const Symbol& lh, const std::string& rh);
bool operator!=(const std::string& lh, const Symbol& rh);

std::ostream& operator<<(std::ostream& os, const Symbol& rh);
