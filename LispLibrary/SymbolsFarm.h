#pragma once
#include <string>
#include <iostream>
#include <unordered_map>
#include <list>
#include <optional>
#include "json/include/json.hpp"

class Cell;
class Symbol;
class SymbolsFarm {
public:
    using oblist = std::list<std::string_view>;
    using symb_mp = std::unordered_map<std::string, std::pair<oblist::iterator, size_t>>;

    class symbol_core {
    public:
        symbol_core();
        symbol_core(symb_mp::iterator it, SymbolsFarm& owner);

        symbol_core(symbol_core&& rh)noexcept;
        symbol_core(const symbol_core& rh);
        ~symbol_core();
        const char* str() const;
        size_t size() const;

        bool is_nil_symbol() const;

        symbol_core& operator=(symbol_core&& rh) noexcept;
        symbol_core& operator=(const symbol_core& rh);
        bool operator==(const symbol_core& rh) const;

        bool empty() const;
        void clear();
    private:
        symb_mp::iterator t_base;
        SymbolsFarm* t_owner;
    };
public:
    SymbolsFarm();
    ~SymbolsFarm();

    void make_or_copy(Symbol& s, std::string&& str);
    void make_or_copy(Symbol& s, const std::string& str);
    void del_symbol(symb_mp::iterator);

    const oblist& get_lst() const;


    void init(std::optional<std::reference_wrapper<nlohmann::json>> state);
    // сохрание состояния
    void save_state(nlohmann::json& j);
    // загрузка состояния 
    void load_state(const nlohmann::json& j);
private:
    symb_mp t_symbol_to_it_and_count;
    oblist t_oblist;
    symbol_core t_nil;
};