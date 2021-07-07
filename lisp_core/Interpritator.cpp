#include "Interprintator.h"

#include <list>
#include <iostream>
#include <unordered_map>

std::unordered_map < std::string, lambda_cell> lambdas;
cell_env vars;

void init_lisp(){
    vars.emplace("nil", nil);
    vars.emplace("T", T);
}


cell bool_cast(bool val) {
    if (val)  return T;
    return nil;
}

bool is_null(const cell& c){
    if (!c.is_list()) return false;
    return c.to_list().empty();
}

bool is_T(const cell& c){
    if (!c.is_atom()) return false;
    if (!is_symbol(c.to_atom().c_str())) return false;
    return c.to_atom() == T.to_atom();
}

bool is_symbol_c(const cell& c) {
    if (!c.is_atom()) return false;
    return is_symbol(c.to_atom().c_str());
}

bool is_number_c(const cell& c) {
    if (!c.is_atom()) return false;
    return is_number(c.to_atom().c_str());
}

cell bifunc_atomp(const std::vector<cell>& c, cell_env& sub_env) {
    return bool_cast(eval(c[1], sub_env).is_atom());
}

cell bifunc_symbolp(const std::vector<cell>& c, cell_env& sub_env) {
    return bool_cast(is_symbol_c(eval(c[1], sub_env)));
}

cell bifunc_numberp(const std::vector<cell>& c, cell_env& sub_env) {
    return bool_cast(is_number_c(eval(c[1], sub_env)));
}

cell bifunc_listp(const std::vector<cell>& c, cell_env& sub_env) {
    return bool_cast(eval(c[1], sub_env).is_list());
}

cell bifunc_null(const std::vector<cell>& c, cell_env& sub_env) {
    return bool_cast(is_null(eval(c[1], sub_env)));
}

cell bifunc_add(const std::vector<cell>& c, cell_env& sub_env)
{
    long n = 0;
    for (auto it = begin(c) + 1; it != end(c); ++it) {
        auto buf = eval(*it, sub_env);
        if (!is_number_c(buf)) throw "bifunc_add error";
        n += atol(buf.to_atom().c_str());
    }
    return make_atom(std::to_string(n));
}

cell  bifunc_sub(const std::vector<cell>& c, cell_env& sub_env)
{
    auto buf = eval(c[1], sub_env);
    if (!is_number_c(buf)) throw "bifunc_sub error";
    long n(atol(buf.to_atom().c_str()));
    for (auto it = begin(c) + 2; it != end(c); ++it) {
        buf = eval(*it, sub_env);
        if (!is_number_c(buf)) throw "bifunc_sub error";
        n -= atol(buf.to_atom().c_str());
    }
    return make_atom(std::to_string(n));
}

cell  bifunc_mul(const std::vector<cell>& c, cell_env& sub_env)
{
    long n = 1;
    for (auto it = begin(c) + 1; it != end(c); ++it) {
        auto buf = eval(*it, sub_env);
        if (!is_number_c(buf)) throw "bifunc_mul error";
        n *= atol(buf.to_atom().c_str());
    }
    return make_atom(std::to_string(n));
}

cell  bifunc_div(const std::vector<cell>& c, cell_env& sub_env)
{
    auto buf = eval(c[1], sub_env);
    if (!is_number_c(buf))throw "bifunc_div error";
    long n(atol(buf.to_atom().c_str()));
    for (auto it = begin(c) + 2; it != end(c); ++it) {
        buf = eval(*it, sub_env);
        if (!is_number_c(buf))throw "bifunc_div error";
        n /= atol(buf.to_atom().c_str());
    }
    return make_atom(std::to_string(n));
}

cell  bifunc_greater(const std::vector<cell>& c, cell_env& sub_env)
{
    auto buf1 = eval(c[1], sub_env);
    auto buf2 = eval(c[2], sub_env);
    if (!is_number_c(buf1) || !is_number_c(buf2)) throw "bifunc_equal error";
    auto result = (atol(buf1.to_atom().c_str()) > atol(buf2.to_atom().c_str()));
   
    for (auto it = begin(c) + 3; it != end(c); ++it) {
        eval(*it, sub_env);
    }
    
    return bool_cast(result);
}

cell  bifunc_less(const std::vector<cell>& c, cell_env& sub_env)
{
    auto buf1 = eval(c[1], sub_env);
    auto buf2 = eval(c[2], sub_env);
    if (!is_number_c(buf1) || !is_number_c(buf2)) throw "bifunc_equal error";
    auto result = (atol(buf1.to_atom().c_str()) < atol(buf2.to_atom().c_str()));
   
    for (auto it = begin(c) + 3; it != end(c); ++it) {
        eval(*it, sub_env);
    }
    
    return bool_cast(result);
}

cell  bifunc_less_equal(const std::vector<cell>& c, cell_env& sub_env)
{
    auto buf1 = eval(c[1], sub_env);
    auto buf2 = eval(c[2], sub_env);
    if (!is_number_c(buf1) || !is_number_c(buf2)) throw "bifunc_equal error";
    auto result = (atol(buf1.to_atom().c_str()) <= atol(buf2.to_atom().c_str()));
   
    for (auto it = begin(c) + 3; it != end(c); ++it) {
        eval(*it, sub_env);
    }
    
    return bool_cast(result);
}

cell  bifunc_equal(const std::vector<cell>& c, cell_env& sub_env)
{
    auto buf1 = eval(c[1], sub_env);
    auto buf2 = eval(c[2], sub_env);
    if (!is_number_c(buf1) || !is_number_c(buf2)) throw "bifunc_equal error";
    auto result = (atol(buf1.to_atom().c_str()) == atol(buf2.to_atom().c_str()));
   
    for (auto it = begin(c) + 3; it != end(c); ++it) {
        eval(*it, sub_env);
    }

    return bool_cast(result);
}

cell bifunc_car(const std::vector<cell>& c, cell_env& sub_env) 
{
    auto buf = eval(c[1], sub_env);
    if (!buf.is_list()) return nil;
    for (auto it = begin(c) + 2; it != end(c); ++it) {
        eval(*it, sub_env);
    }
    return buf.to_list()[0];
}

cell bifunc_cdr(const std::vector<cell>& c, cell_env& sub_env)
{
    auto buf = eval(c[1], sub_env);
    if (!buf.is_list()) return nil;
    for (auto it = begin(c) + 2; it != end(c); ++it) {
        eval(*it, sub_env);
    }
    const auto& lst = buf.to_list();
    return make_list(std::vector<cell>(next(begin(lst)), end(lst)));
}

cell  bifunc_append(const std::vector<cell>& c, cell_env& sub_env)
{
    std::list<cell> lst;
    for (auto it = begin(c) + 1; it != end(c); ++it) {
        auto buf = eval(*it, sub_env);
        if (!buf.is_list()) return nil;
        for (const auto& elem : buf.to_list()) {
            lst.push_back(elem);
        }
    }
    return make_list(std::move(lst));
}

cell  bifunc_cons(const std::vector<cell>& c, cell_env& sub_env)
{
    auto buf = eval(c[2], sub_env);
    if (!buf.is_list()) return nil;
    const auto& lst = buf.to_list();
    std::vector<cell> result(1 + lst.size());
    result[0] = eval(c[1], sub_env);
    for(unsigned char  i = 1; i != result.size(); ++i){
        result[i] = lst[i-1];
    }
    for (auto it = begin(c) + 3; it != end(c); ++it) {
        eval(*it, sub_env);
    }
    return make_list(std::move(result));
}

cell  bifunc_list(const std::vector<cell>& c, cell_env& sub_env)
{
    std::vector<cell> lst(c.size()-1);
    for(unsigned char  i = 0; i < c.size(); ++i){
        lst[i] = eval(c[i+1], sub_env);
    }
    return make_list(std::move(lst));
}

cell  nbifunc_quote(const std::vector<cell>& c, cell_env& sub_env)
{
    if (next(begin(c)) == end(c)) throw "bifunc_quote";
    return c[1];
}

cell  nbifunc_defun(const std::vector<cell>& c, cell_env& sub_env) {
    //FREERAM_PRINT_TEXT( "defun "));
    if (!is_symbol_c(c[1])) return bool_cast(false);
    if (!c[2].is_list()) return bool_cast(false);
    const auto& name = c[1].to_atom();
    const auto& first_lst = c[2].to_list();

    std::vector<cell> params;
    const cell* body = nullptr;

    if (first_lst.empty()) {
        if (begin(c) + 3 == end(c)) throw "bifunc_defun";
        body = &(*(begin(c) + 3));
    }
    else {
        //FREERAM_PRINT_TEXT( "w_args "));
        if (!is_symbol_c(first_lst[0])) throw "bifunc_defun";


        const auto& name = first_lst[0].to_atom() ;
        if (name == "nlambda") {
            if (!first_lst[1].is_list()) throw "bifunc_defun";
            params = first_lst[1].to_list();

            body = &(first_lst[2]);
           /* if (lambdas.find(name) == std::end(lambdas)) {
                lambdas[name] = make_nlambda(std::move(params), *body);
            }
            else {
                lambdas.get(name) = make_nlambda(std::move(params), *body);
            }*/
            lambdas[name] = make_nlambda(std::move(params), *body);
            return c[1];
        }
        else if (name == "lambda") {
            if (!first_lst[1].is_list()) throw "bifunc_defun";
            params = first_lst[1].to_list();

            body = &(first_lst[2]);
        }
        else {
            params = first_lst;
            body = &(c[3]);
        }
    }
    //FREERAM_PRINT_TEXT( "find ");
    //if (!lambdas.find(name)) {
    //    //FREERAM_PRINT_TEXT( "not found "));
    //    lambdas.add(name, make_lambda(std::move(params), *body));
    //}
    //else {
    //    //FREERAM_PRINT_TEXT( "found "));
    //    lambdas.get(name) = make_lambda(std::move(params), *body);
    //}
    lambdas[name] = make_lambda(std::move(params), *body);
    //FREERAM_PRINT_TEXT( "return "));
    return c[1];
}

cell eval_atom(const cell& atom, cell_env& sub_env) {
    if (is_symbol_c(atom)) {
        if (vars.find(atom.to_atom()) != std::end(vars)) {
            return vars.at(atom.to_atom());
        }
        else {
            const auto& name = atom.to_atom();
            if (sub_env.find(name) != std::end(sub_env)) {
                return sub_env.at(name);
            }
        }
    }
    return atom;
}

std::string to_string2(const cell& exp)
{
    if (exp.is_list()) {
        const auto& lst = exp.to_list();
        if (lst.empty()) return "nil";
        std::string s("(");
        for (auto e = begin(lst); e != end(lst); ++e) {
            s += to_string2(*e);
            s += ' ';
        }

        if (s[s.length() - 1] == ' ') {
            s.erase(s.length() - 1);
        }
        s += ')';
        return s;
    }
    else if (is_symbol(exp.to_atom().c_str())) {
        return exp.to_atom();
    }
    else {
        return exp.to_atom();
    }
}

cell eval(const cell& arg, cell_env& sub_env) {
    //FREERAM_PRINT_TEXT( "ev "));
    //FREERAM_PRINT_TEXT(to_string2(arg).c_str());
    if (arg.is_list()) {
        const auto& lst = arg.to_list();
        if (!is_symbol_c(lst[0])) throw "eval error";

        {
            auto func = find_bifunc(lst[0].to_atom());
            if (!func.empty()) {
                if(func.is_bifunc()){
                    return func.to_bifunc()(lst, sub_env);
                }
                return func.to_nbifunc()(lst, sub_env);
            }
        }
        
        if (lambdas.find(lst[0].to_atom()) != std::end(lambdas)) {
            cell_env params;
            const auto& fnc = lambdas.at(lst[0].to_atom());
            if(fnc.is_lambda()){
                {
                    const auto& lambda = fnc.to_lambda();
                    auto it = next(begin(lst));
                    for (auto arg_it = begin(lambda.params); arg_it != end(lambda.params); ++arg_it) {
                        params.emplace(arg_it->to_atom(),  eval(*it, sub_env));
                        ++it;
                    }

                    for (; it != end(lst); ++it) {
                        eval(*it, sub_env);
                    }
                }
                return eval(fnc.to_lambda().body, params);
            }
            else{
                {
                    const auto& lambda = fnc.to_nlambda();
                    auto it = next(begin(lst));
                    for (auto arg_it = begin(lambda.params); arg_it != end(lambda.params); ++arg_it) {
                        params.emplace(arg_it->to_atom(), *it);
                        ++it;
                    }
                }
                return eval(fnc.to_nlambda().body, params);
            }
        }
        else {
            throw "eval error";
        }
    }
    else {
        return eval_atom(arg, sub_env);
    }
}

std::pair<bool, cell>  cond_exec_arc(const std::vector<cell>& c, cell_env& sub_env){
     if (c[0].is_list()){
        if(is_null(c[0])) return  {false, nil};
        
        if(is_null(eval(c[0], sub_env))) return  {false, nil};
        else{
            return  {true, eval(c[1], sub_env)};
        }
     }
     else if(is_T(eval(c[0], sub_env))){
        return {true, eval(c[1], sub_env)};
     }
     else{
         throw "cond_exec_arc";
     }
}

cell  nbifunc_cond(const std::vector<cell>& c, cell_env& sub_env)
{
    for (auto it = next(begin(c)); it != end(c); ++it) {
        const auto& elem = *it;
        if(!elem.is_list()) throw "bifunc_cond";
        auto result_pair = cond_exec_arc(elem.to_list(), sub_env);
        if(result_pair.first){
            return result_pair.second;
        }
    }
    throw "bifunc_cond";
}

cell  nbifunc_if(const std::vector<cell>& c, cell_env& sub_env)
{
    if (!is_null(eval(c[1], sub_env))) {
        return eval(c[2], sub_env);
    }
    else {
        return eval(c[3], sub_env);
    }
}

cell  nbifunc_setq(const std::vector<cell>& c, cell_env& sub_env)
{
    if (!is_symbol_c(c[1])) throw "bifunc_setq";
    const auto& name = c[1].to_atom();
    auto val = eval(c[2], sub_env);
    if (vars.find(name) != std::end(vars)) {
        vars[name] = val;
    }
    else {
        if (sub_env.find(name) != std::end(sub_env)) {
            sub_env[name] = val;
        }
        else {
            vars.emplace(name, val);
        }
    }
    return val;
}

cell  nbifunc_eval(const std::vector<cell>& c, cell_env& sub_env) {
    return eval(c[1], sub_env);
}

bifunc_cell find_bifunc(const std::string& str){
    if(str == "append"){ return  make_bifunc(&bifunc_append);}
    if(str == "car"){ return  make_bifunc(&bifunc_car);}
    if(str == "cdr"){ return  make_bifunc(&bifunc_cdr);}
    if(str == "cons"){ return  make_bifunc(&bifunc_cons);}
    if(str == "list"){ return  make_bifunc(&bifunc_list);}
    if(str == "null"){ return  make_bifunc(&bifunc_null);}
    if(str == "numberp"){ return  make_bifunc(&bifunc_numberp);}
    if(str == "symbolp"){ return  make_bifunc(&bifunc_symbolp);}
    if(str == "listp"){ return  make_bifunc(&bifunc_listp);}
    if(str == "atomp"){ return  make_bifunc(&bifunc_atomp);}

    if(str == "+"){ return  make_bifunc(&bifunc_add);}
    if(str == "-"){ return  make_bifunc(&bifunc_sub);}
    if(str == "*"){ return  make_bifunc(&bifunc_mul);}
    if(str == "/"){ return  make_bifunc(&bifunc_div);}
    if(str == ">"){ return  make_bifunc(&bifunc_greater);}
    if(str == "<"){ return  make_bifunc(&bifunc_less);}
    if(str == "<="){ return  make_bifunc(&bifunc_less_equal);}
    if(str == "="){ return  make_bifunc(&bifunc_equal);}

    if(str == "eval"){ return  make_nbifunc(&nbifunc_eval);}
    if(str == "defun"){ return  make_nbifunc(&nbifunc_defun);}
    if(str == "quote"){ return  make_nbifunc(&nbifunc_quote);}
    if(str == "cond"){ return  make_nbifunc(&nbifunc_cond);}
    if(str == "if"){ return  make_nbifunc(&nbifunc_if);}
    if(str == "setq"){ return  make_nbifunc(&nbifunc_setq);}

    return  bifunc_cell();
}