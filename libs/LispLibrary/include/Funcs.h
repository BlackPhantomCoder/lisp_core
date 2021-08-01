#pragma once
#include "Cell.h"
#include <list>
#include <string>
#include <iostream>
#include "PublicCoreEnvironmentProvider.h"
#include "SymbolsProvider.h"

std::string to_string(const Cell& exp);

Cell bool_cast(bool val, SymbolsProvider& provider);
bool is_null(const Cell& c, SymbolsProvider& provider);
bool is_T(const Cell& c, SymbolsProvider& provider);
bool is_implicit_cond(const Cell& arg, SymbolsProvider& provider);