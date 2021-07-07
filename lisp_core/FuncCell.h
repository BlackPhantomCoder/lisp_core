#pragma once
#include "Cell.h"
#include <unordered_map>
#include <vector>

enum class bifunc_types : bool {bifunc, nbifunc};

class bifunc_cell {
public:
    typedef cell(*bifunc2)(const std::vector<cell>&, std::unordered_map <std::string, cell>&);
    typedef cell(*nbifunc2)(const std::vector<cell>&, std::unordered_map <std::string,  cell>&);
public:
    bifunc_cell(const bifunc_cell& rh) =default;

    bifunc_cell():
        t_type(bifunc_types::bifunc),
        t_func(nullptr)
    {

    }

    bifunc_cell(bifunc_types type, void* func):
        t_type(type),
        t_func(func)
    {

    }
    ~bifunc_cell(){
        t_func = nullptr;
    }

    bool is_bifunc()const { return  (!empty()) && (t_type == bifunc_types::bifunc);}
    bool is_nbifunc()const  { return  (!empty()) && (t_type == bifunc_types::nbifunc);}

    bifunc_cell& operator=(const bifunc_cell& rh) =default;

    bifunc2 to_bifunc()const {if(!is_bifunc()) throw "to_bifunc"; return (bifunc2)t_func;}
    nbifunc2 to_nbifunc()const {if (!is_nbifunc()) throw "to_nbifunc"; return (nbifunc2)t_func;}

    bool empty() const {return !t_func; }
private:
    bifunc_types t_type;
    void* t_func;
};

struct ilambda{
    ilambda(std::vector<cell>&& params, const cell& body) :
        params(std::move(params)),
        body(body)
    {}
    ilambda(const std::vector<cell>& params, const cell& body) :
        params(params),
        body(body)
    {}
    //args symbs
    std::vector<cell> params;
    cell body;
};


enum class lambda_types : bool {lambda, nlambda};

class lambda_cell {
public:
    lambda_cell(const lambda_cell& rh) =default;

    lambda_cell():
        t_type(lambda_types::lambda),
        t_func(std::vector<cell>(), cell())
    {

    }

    lambda_cell(lambda_types type, const ilambda& func):
        t_type(type),
        t_func(func)
    {
    }
    ~lambda_cell(){
        t_func = {std::vector<cell>(), cell()};
    }

    bool is_lambda()const { return  (t_func.body.is_list() || t_func.body.is_atom()) && (t_type == lambda_types::lambda);}
    bool is_nlambda()const  { return  (t_func.body.is_list() || t_func.body.is_atom()) && (t_type == lambda_types::nlambda);}

    lambda_cell& operator=(const lambda_cell& rh) =default;

    ilambda& to_lambda() {if (!is_lambda()) throw "to_lambda"; return t_func;}
    const ilambda& to_lambda() const{if(!is_lambda()) throw "to_lambda"; return t_func;}

    
    ilambda& to_nlambda() {if (!is_nlambda()) throw "to_nlambda"; return t_func;}
    const ilambda& to_nlambda() const{if (!is_nlambda()) throw "to_nlambda"; return t_func;}
private:
    lambda_types t_type;
    ilambda t_func;
};

lambda_cell make_lambda(std::vector<cell>&& params, const cell& body);
lambda_cell make_lambda(const std::vector<cell>& params, const cell& body);

lambda_cell make_nlambda(std::vector<cell>&& params, const cell& body);
lambda_cell make_nlambda(const std::vector<cell>& params, const cell& body);

bifunc_cell make_bifunc(bifunc_cell::bifunc2 func);
bifunc_cell make_nbifunc(bifunc_cell::nbifunc2 func);