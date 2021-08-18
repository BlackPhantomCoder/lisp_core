#pragma once
#include "SExpr.h"
#include "SymbolsFarm.h"
#include "SExprsFarm.h"


class Symbol : public SExpr {
    friend class SExprsFarm;
    friend class SymbolsFarm;
    friend bool is_null_symbol(const Symbol& c);
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


    virtual bool is_symbol() const override;
    virtual bool is_atom() const override;
    virtual SExpr::del_func_ptr get_del_fnc() const override;
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