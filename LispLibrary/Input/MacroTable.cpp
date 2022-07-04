#include "MacroTable.h"
#include "CoreEnv.h"
using namespace std;

MacroTable::MacroTable(CoreEnvironment& env):
	t_env(env)
{
}

void MacroTable::init(std::optional<std::reference_wrapper<nlohmann::json>> state)
{
	if (state) {
		load_state(*state);
	}
}

void MacroTable::set_macro_char(char ch, macro_char mch) {
	t_table[ch] = mch;
}

std::optional<std::reference_wrapper<const macro_char>> MacroTable::is_macro_char(char ch) const
{
	if (auto it = t_table.find(ch); it != std::end(t_table)) {
		return it->second;
	}
	return std::nullopt;
}

void MacroTable::save_state(nlohmann::json& j)
{
	auto& jtable = j.emplace_back();

	for (const auto& [ch, val] : t_table) {
		auto& elem = jtable.emplace_back();
		//+48 чтобы не делало \uxxxx
		elem.emplace_back(""s + ch + char(char(val.type) + 48));
		t_env.cserial().out(elem.emplace_back(), val.func);
	}
}

void MacroTable::load_state(const nlohmann::json& j)
{
	auto& jtable = j.at(0);

	t_table.clear();
	for (const auto& elem : jtable) {
		auto str = elem.at(0).get<string>();
		t_table[str[0]] = macro_char{
			macro_char::type_t(str[1]- 48),
			t_env.cserial().in(elem.at(1))
		};
	}
}
