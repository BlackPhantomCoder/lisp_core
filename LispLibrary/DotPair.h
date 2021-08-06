#pragma once
#include <list>
#include "Cell.h"
#include <initializer_list>
#include <optional>
#include "SymbolsProvider.h"
class Cell;

struct DotPairCore {
    DotPairCore(const Cell& f, const Cell& s) :
        t_f(f),
        t_s(s)
    {

    }
    Cell t_f;
    Cell t_s;
};

class DotPair
{
    friend inline Cell& car(DotPair& dp);
    friend inline Cell& cdr(DotPair& dp);
    friend inline const Cell& car(const DotPair& dp);
    friend inline const Cell& cdr(const DotPair& dp);
    friend inline Cell& car(Cell& dp);
    friend inline Cell& cdr(Cell& dp);
    friend inline const Cell& car(const Cell& dp);
    friend inline const Cell& cdr(const Cell& dp);

    friend inline DotPair cons(const Cell& f, const Cell& s, SymbolsProvider& provider);
    friend inline Cell cons_cell(const Cell& f, const Cell& s, SymbolsProvider& provider);

    //DotPair append(const Cell& f, const Cell& s);
    friend inline Cell append_cell(const Cell& f, const Cell& s, SymbolsProvider& provider);

    friend inline DotPair make_empty_list(SymbolsProvider& provider);
    friend inline Cell make_empty_list_cell(SymbolsProvider& provider);

    template< class InputIt >
    friend inline DotPair make_list(InputIt first, InputIt last, SymbolsProvider& provider);
    friend inline DotPair make_list(std::initializer_list<Cell> l, SymbolsProvider& provider);
    friend inline DotPair deep_copy_list(const DotPair& l);

    friend inline bool is_null(const DotPair& dp);

    friend inline std::vector<Cell> copy_dpair_list_to_vector(const Cell& dp, bool skip_last_pair);
    friend inline std::vector<Cell> copy_dpair_list_to_vector(const DotPair& dp, bool skip_last_pair);

public:
    DotPair(const DotPair& rh) = default;
    DotPair(DotPair&& rh)noexcept = default;
    ~DotPair() = default;
    bool empty() const;

    DotPair& operator=(const DotPair& rh) = default;
    DotPair& operator=(DotPair&& rh)noexcept = default;
private:
    DotPair(SymbolsProvider& provider);
    DotPair(DotPairCore&& dpc, SymbolsProvider& provider);
    DotPair(const DotPairCore& dpc, SymbolsProvider& provider);
private:
    std::shared_ptr<DotPairCore> t_core;
    std::reference_wrapper<SymbolsProvider> t_symbols;
};

inline Cell& car(DotPair& dp);
inline Cell& cdr(DotPair& dp);
inline const Cell& car(const DotPair& dp);
inline const Cell& cdr(const DotPair& dp);
inline Cell& car(Cell& dp);
inline Cell& cdr(Cell& dp);
inline const Cell& car(const Cell& dp);
inline const Cell& cdr(const Cell& dp);

inline DotPair cons(const Cell& f, const Cell& s, SymbolsProvider& provider);
inline Cell cons_cell(const Cell& f, const Cell& s, SymbolsProvider& provider);

//DotPair append(const Cell& f, const Cell& s);
inline Cell append_cell(const Cell& f, const Cell& s, SymbolsProvider& provider);

inline DotPair make_empty_list(SymbolsProvider& provider);
inline Cell make_empty_list_cell(SymbolsProvider& provider);

template< class InputIt >
inline DotPair make_list(InputIt first, InputIt last, SymbolsProvider& provider);
inline DotPair make_list(std::initializer_list<Cell> l, SymbolsProvider& provider);
inline DotPair deep_copy_list(const DotPair& l);

inline bool is_null(const DotPair& dp);

inline std::vector<Cell> copy_dpair_list_to_vector(const Cell& dp, bool skip_last_pair = false);
inline std::vector<Cell> copy_dpair_list_to_vector(const DotPair& dp, bool skip_last_pair = false);