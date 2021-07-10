#pragma once
#include "Cell.h"
#include <list>
#include <string>
#include <iostream>

// convert given string to list of tokens
std::list<std::string> tokenize(const std::string& str);
// return the Lisp expression in the given tokens
Cell read_from(std::list<std::string>& tokens);
// return the Lisp expression represented by the given string
Cell read(const std::string& s);
// convert given Cell to a Lisp-readable string
std::string to_string(const Cell& exp);


std::vector<Cell> listp_expressions_from_string(const std::string& str);


std::vector<Cell> listp_expressions_from_stream(std::istream& in, bool skip_comments = false);