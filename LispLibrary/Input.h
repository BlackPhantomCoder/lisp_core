#pragma once
#include <optional>
#include "Cell.h"
#include "StdCoreInputStream.h"

class CoreEnvironment;

// класс для простого чтения из потока
class Input
{
public:
	Input(CoreEnvironment& env);

	bool alive();
	//PEEK-FLAG
	int  read_char(bool flag = false);
	int  peek_char();
	void unread_char();

	bool read_upcase_val();
	int  read_base_val();

	void change_cis(CoreInputStreamInt& new_cis);
private:
	CoreEnvironment& t_env;
	std::optional<std::reference_wrapper<CoreInputStreamInt>> t_cis;
	bool t_ratom_read_buf = true;
	Cell t_read_upcase;
	Cell t_read_base;
};