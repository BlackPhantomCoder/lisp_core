#pragma once
#include "Cell.h"
#include "SExprsFarm.h"

class OutputController
{
public:
	OutputController(SExprsFarm& farm);

	std::string to_string(const Cell& exp);

	void set_read_upcase(bool val);
private:
	std::string t_to_string_list(const Cell& d);
	std::string t_to_string(const Symbol& s);
	std::string t_to_string(const Number& n);
private:
	SExprsFarm& t_farm;
	bool t_read_upcase = true;
};

