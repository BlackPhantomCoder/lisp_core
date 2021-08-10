#pragma once
#include "SymbolsFarm.h"
#include "SExprsFarm.h"


class Symbol {
    friend class SExprsFarm;
    friend class SymbolsFarm;
    //friend Symbol make_symbol(const std::string& data, SymbolsFarm& owner);
    //friend Symbol make_symbol(std::string&& data, SymbolsFarm& owner);
    //friend Symbol make_symbol(const std::string& data, SExprsFarm& owner);
    //friend Symbol make_symbol(std::string&& data, SExprsFarm& owner);
public:
    Symbol() = default;
    ~Symbol();

    Symbol(Symbol&& rh) noexcept = default;
    Symbol(const Symbol& rh) = default;


    Symbol& operator=(Symbol&& rh) noexcept = default;
    Symbol& operator=(const Symbol& rh) = default;

    bool operator<(const Symbol& symb)const;
    bool operator==(const Symbol& symb)const;
    bool operator!=(const Symbol& symb)const;

    size_t size()const;
    const char* to_string()const;

    bool empty() const;
    void clear();
private:
    Symbol(const SymbolsFarm::symbol_core& data);
private:
    SymbolsFarm::symbol_core t_data;
};

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

bool operator==(const char* lh, const Symbol& rh);
bool operator==(const Symbol& lh, const char* rh);
bool operator==(const Symbol& lh, const std::string& rh);
bool operator==(const std::string& lh, const Symbol& rh);

bool operator!=(const Symbol& lh, const char* rh);
bool operator!=(const char* lh, const Symbol& rh);
bool operator!=(const Symbol& lh, const std::string& rh);
bool operator!=(const std::string& lh, const Symbol& rh);


//
//Symbol make_symbol(Symbol&& rh);
//Symbol make_symbol(const Symbol& rh);
//Symbol make_symbol(const std::string& data, SExprsFarm& owner);
//Symbol make_symbol(std::string&& data, SExprsFarm& owner);
//Symbol make_symbol(const std::string& data, SymbolsFarm& owner);
//Symbol make_symbol(std::string&& data, SymbolsFarm& owner);