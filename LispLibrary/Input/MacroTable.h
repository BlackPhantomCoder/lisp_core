#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <optional>
#include <iostream>
#include "json/include/json.hpp"
#include "Cell.h"

struct macro_char {
	enum class type_t : char{
		terminating = 33,
		non_terminating,
		comment
	};

	type_t type;
	Cell func;

	bool is_terminating() const { return this->type == type_t::terminating; }
	bool is_non_terminating() const { return this->type == type_t::non_terminating; }
	bool is_comment() const { return this->type == type_t::comment; }
};

class CoreEnvironment;
class MacroTable
{
public:
	MacroTable(CoreEnvironment& env);

	void init(std::optional<std::reference_wrapper<nlohmann::json>> state);

	void set_macro_char(char ch, macro_char mch);

	std::optional<std::reference_wrapper<const macro_char>> is_macro_char(char ch) const;

	// сохрание состояния
	void save_state(nlohmann::json& j);
	// загрузка состояния 
	void load_state(const nlohmann::json& j);
private:
	CoreEnvironment& t_env;
	std::unordered_map<char, macro_char> t_table;
};

