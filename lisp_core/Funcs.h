#pragma once
#include "Cell.h"
#include <list>
#include <string>

// convert given string to list of tokens
std::list<std::string> tokenize(const std::string& str);
// return the Lisp expression in the given tokens
Cell read_from(std::list<std::string>& tokens);
// return the Lisp expression represented by the given string
Cell read(const std::string& s);
// convert given Cell to a Lisp-readable string
std::string to_string(const Cell& exp);

// convert given Cell to a Lisp-readable string
std::vector<std::string> expressions_from_file(const std::string& path);