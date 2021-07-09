#pragma once
#include "Cell.h"
#include <unordered_map>
#include <vector>
#include <functional>

enum class bifunc_types : bool {bifunc, nbifunc};

class BIFuncCell {
public:
    using fnc = std::function<Cell(const std::vector<Cell>&, std::unordered_map <std::string, Cell>&)>;
public:
    BIFuncCell() = default;
    BIFuncCell(const BIFuncCell& rh) = default;
    BIFuncCell(bifunc_types type, fnc func);
    ~BIFuncCell();

    BIFuncCell& operator=(const BIFuncCell& rh) =default;

    bool is_bifunc()const;
    bool is_nbifunc()const;

    fnc get();
    bool empty() const;
private:
    bifunc_types t_type = bifunc_types::bifunc;
    fnc t_func;
};

BIFuncCell make_bifunc(BIFuncCell::fnc func);
BIFuncCell make_nbifunc(BIFuncCell::fnc func);