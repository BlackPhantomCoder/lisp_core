#pragma once
#include "Cell.h"
#include <list>
#include <string>
#include <iostream>
#include "PublicCoreEnvironmentProvider.h"
#include "Types.h"

// return the Lisp expression in the given tokens
Cell read_from(std::list<std::string>& tokens, const IPublicCoreEnvironmentProvider& env);
// convert given Cell to a Lisp-readable string
std::string to_string(const Cell& exp);

std::pair<bool, Cell> read_one_expression_from_string(
	const std::string& str,
	const IPublicCoreEnvironmentProvider& env,
	bool skip_comments = false
);

std::pair<bool, Cell> read_one_expression_from_stream(
	std::istream& in,
	const IPublicCoreEnvironmentProvider& env,
	stream_read_mode mode = stream_read_mode::new_string,
	bool skip_comments = false
);



std::vector<Cell> read_expressions_from_string(
	const std::string& str,
	const IPublicCoreEnvironmentProvider& env,
	bool skip_comments = false
);

std::vector<Cell> read_expressions_from_stream(
	std::istream& in,
	const IPublicCoreEnvironmentProvider& env,
	stream_read_mode mode = stream_read_mode::new_string,
	bool skip_comments = false
);



Cell bool_cast(bool val);
bool is_null(const Cell& c);
bool is_symbol_c(const Cell& c);
bool is_integer_number_c(const Cell& c);
bool is_real_number_c(const Cell& c);
bool is_number2_c(const Cell& c);
bool is_T(const Cell& c);
bool is_implicit_cond(const Cell& arg);