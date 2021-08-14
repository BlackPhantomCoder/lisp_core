#pragma once
#include "Cell.h"
#include "DotPair.h"
#include "SExprsFarm.h"
#include "Number.h"
#include "Symbol.h"
#include "CoreEnv.h"

#include <list>
#include <string>
#include <iostream>
#include <deque>

bool is_special_symbol(bool read_upcase, unsigned char token);

Cell bool_cast(bool val, SExprsFarm& farm);
bool is_null(const Cell& c);
bool is_null_symbol(const Symbol& c);
bool is_null_symbol(const Cell& c);

bool is_implicit_cond(const Cell& arg, SExprsFarm& farm);

bool is_alambda_form(const Cell& arg, SExprsFarm& farm);
bool is_lambda_form(const Cell& arg, SExprsFarm& farm);
bool is_nlambda_form(const Cell& arg, SExprsFarm& farm);


//not if dp is atom -> result == dp
Cell& car(Cell& dp);
Cell& cdr(Cell& dp);
const Cell& car(const Cell& dp);
const Cell& cdr(const Cell& dp);

Cell cons(const Cell& f, const Cell& s, SExprsFarm& farm);
template<class InputIt>
Cell cons_range(InputIt beg_it, InputIt end_it, SExprsFarm& farm);

Cell append(const Cell& f, const Cell& s, SExprsFarm& farm);

void rplaca(DotPair& rh, const Cell& exp);
void rplacd(DotPair& rh, const Cell& exp);

Cell tree_copy(const Cell& rh, SExprsFarm& farm);












template<class InputIt>
inline Cell SExprsFarm::make_list_cell(InputIt first, InputIt last)
{
    if (first == last) return this->make_empty_list_cell();
    if (std::next(first) == last) return cons(*first, this->make_empty_list_cell(), *this);
    if constexpr (std::is_convertible<std::iterator_traits<InputIt>::iterator_category, std::bidirectional_iterator_tag >::value) {
        Cell result = this->make_empty_list_cell();
        auto r_end = std::make_reverse_iterator(first);
        auto r_beg = std::make_reverse_iterator(last);

        for (; r_beg != r_end; ++r_beg) {
            result = cons(*r_beg, result, *this);
        }
        return result;
    }
    else {
        Cell result = this->make_empty_list_cell();
        std::vector<const Cell*, CoreData::allocator<const Cell*>> stack;
        while (first != last) {
            stack.push_back(&*first);
            ++first;
        }
        for (auto it = std::rbegin(stack); it != std::rend(stack); ++it) {
            result = cons(*(*it), result, *this);
        }
        return result;
    }
}

template<class InputIt>
inline Cell cons_range(InputIt beg_it, InputIt end_it, SExprsFarm& farm)
{
    if (beg_it == end_it) return cons(farm.nil, farm.nil, farm);
    if (std::next(beg_it) == end_it) return cons(*beg_it, farm.nil, farm);
    if constexpr (std::is_convertible<std::iterator_traits<InputIt>::iterator_category, std::bidirectional_iterator_tag >::value) {
        auto r_end = std::make_reverse_iterator(beg_it);
        auto r_beg = std::make_reverse_iterator(end_it);
        Cell result = *r_beg;
        ++r_beg;
        for (; r_beg != r_end; ++r_beg) {
            result = cons(*r_beg, result, farm);
        }
        return result;
    }
    else {
        if (beg_it == end_it) return cons(farm.nil, farm.nil, farm);
        std::vector<const Cell*, CoreData::allocator<const Cell*>> stack;
        while (beg_it != end_it) {
            stack.push_back(&*beg_it);
            ++beg_it;
        }
        Cell result = *(*std::rbegin(stack));
        for (auto it = std::rbegin(stack) + 1; it != std::rend(stack); ++it) {
            result = cons(*(*it), result, farm);
        }
        return result;
    }
}
