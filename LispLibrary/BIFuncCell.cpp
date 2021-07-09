#include "BIFuncCell.h"

BIFuncCell::BIFuncCell(bifunc_types type, fnc func) :
    t_type(type),
    t_func(func)
{

}
BIFuncCell::~BIFuncCell() {
    t_func = nullptr;
}

bool BIFuncCell::is_bifunc()const { 
    return  (!empty()) && (t_type == bifunc_types::bifunc); 
}

bool BIFuncCell::is_nbifunc()const { 
    return  (!empty()) && (t_type == bifunc_types::nbifunc); 
}

BIFuncCell::fnc BIFuncCell::get() {
    return t_func; 
}

bool BIFuncCell::empty() const {
    return !t_func; 
}




BIFuncCell make_bifunc(BIFuncCell::fnc func){
    return BIFuncCell(bifunc_types::bifunc, func);
}

BIFuncCell make_nbifunc(BIFuncCell::fnc func){
    return BIFuncCell(bifunc_types::nbifunc, func);
}
