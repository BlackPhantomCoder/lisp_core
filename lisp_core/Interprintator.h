#pragma once
#include "Cell.h"
#include "FuncCell.h"
#include "Funcs.h"

#include <vector>

using cell_env = std::unordered_map < std::string, cell>;
#define nil make_list({})
#define T make_atom("T")

void init_lisp();

cell bifunc_atomp(const std::vector<cell>& c);
cell bifunc_symbolp(const std::vector<cell>& c);
cell bifunc_numberp(const std::vector<cell>& c);
cell bifunc_listp(const std::vector<cell>& c);
cell bifunc_null(const std::vector<cell>& c);
cell bifunc_add(const std::vector<cell>& c);
cell bifunc_sub(const std::vector<cell>& c);
cell bifunc_mul(const std::vector<cell>& c);
cell bifunc_div(const std::vector<cell>& c);
cell bifunc_greater(const std::vector<cell>& c);
cell bifunc_less(const std::vector<cell>& c);
cell bifunc_less_equal(const std::vector<cell>& c);
cell bifunc_equal(const std::vector<cell>& c);
cell bifunc_car(const std::vector<cell>& c);
cell bifunc_cdr(const std::vector<cell>& c);
cell bifunc_append(const std::vector<cell>& c);
cell bifunc_cons(const std::vector<cell>& c);
cell bifunc_list(const std::vector<cell>& c);
cell nbifunc_quote(const std::vector<cell>& c, cell_env& sub_env);
cell nbifunc_defun(const std::vector<cell>& c, cell_env& sub_env);
std::pair<bool, cell>  cond_exec_arc(const std::vector<cell>& c, cell_env& sub_env);
cell nbifunc_cond(const std::vector<cell>& c, cell_env& sub_env);
cell nbifunc_if(const std::vector<cell>& c, cell_env& sub_env);
cell nbifunc_setq(const std::vector<cell>& c, cell_env& sub_env);
cell nbifunc_eval(const std::vector<cell>& c, cell_env& sub_env);

bifunc_cell find_bifunc(const std::string& str);

cell eval_atom(const cell& atom, cell_env& sub_env);
cell eval(const cell& arg, cell_env& sub_env);