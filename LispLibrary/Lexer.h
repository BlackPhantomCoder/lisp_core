#pragma once
#include "CoreStreamInt.h"
#include "SExprsFarm.h"

#include <vector>
#include <string>
#include <stack>
#include <string>
#include <variant>
#include <functional>
#include <unordered_map>

struct throw_endless_stream_error {};
struct throw_token_stream_error {};
struct throw_token_stream_empty {};

//enum class read_reason { success, stream_fail, wrong_input, empty_input };

class Syntaxer;
class read_helper;

class MacroChars {
public:
	using cont_type = std::variant<std::function<Cell(read_helper&)>, Cell>;
public:
	MacroChars();

	void set_macro_char(char ch, cont_type&& defenition);
	const cont_type& get_macro_char(char ch)const;
	bool is_macro_char(char ch) const;

private:
	std::unordered_map<char, cont_type> t_macrochars;
};

class read_helper {
public:
	read_helper(Syntaxer& s);

	Cell read();

	bool is_macro_char(char ch) const;
	bool is_space_char(char ch) const;
	bool is_comment_char(char ch) const;
	bool is_number_char(char ch) const;

	const CoreInputStreamInt& get_stream()const;
	CoreInputStreamInt& get_stream();
	SExprsFarm& get_farm();
	bool is_skip_comment() const;

	void skip_comments_and_spaces_if();
private:
	Cell t_read_macro_char(char ch);
	Cell t_read_number(std::string&& prev);

	bool t_skip_comments_if();
	bool t_skip_spaces_if();
private:
	Syntaxer& t_s;
};



class SExprStream {
public:
	SExprStream(Syntaxer& s);
	Cell read();
	bool alive() const;
	bool ready() const;
private:
	read_helper t_helper;
};



class CoreEnvironment;

class Syntaxer
{
	friend class SExprStream;
	friend class read_helper;
public:
	Syntaxer(CoreEnvironment& env);

	Cell read_sexpr();
	SExprStream read_sexprs_stream();

	Cell listen() const;
	Cell read_char(const Cell& flag);
	Cell peek_char(const Cell& s);
	void unread_char();

	MacroChars& get_macrochars();
private:
	SExprsFarm& t_farm();

	CoreInputStreamInt& t_stream();
	CoreInputStreamInt& t_stream() const;

	bool t_read_upcase_mode();
	bool t_skip_comments_mode();
	int t_number_base_mode();
private:
	Cell t_rap_buf;
	CoreEnvironment& t_env;
	MacroChars t_mc;
};

