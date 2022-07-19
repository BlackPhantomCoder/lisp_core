#pragma once
#include "SExpr.h"
#include "SymbolsFarm.h"

class Symbol : public SExpr {
    friend class SymbolsFarm;
    friend bool is_null_symbol(const Symbol& c);
public:
    Symbol(const SymbolsFarm::symbol_core& data);
    Symbol(SymbolsFarm::symbol_core&& data);
    ~Symbol() = default;

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