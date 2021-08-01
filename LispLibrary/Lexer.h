#pragma once
#include "Cell.h"
#include "CoreStreamInt.h"
#include <vector>
#include <list>
#include <string>
#include <stack>

enum class tokens {
	OpenBracket,	// (
	CloseBracket,	// )
	Expr,			// выражение
	Quote,			// '
	//DoubleQuote,	// "
	//BackQuote,		// `
	//Comma,			// ,
	//CommercialAt,   // @
	//Dot,			// .
	//Semicolon,		// ;
	//BackSlash,		/* \  */
	//VerticalLine,	// |
	QExpr,			// |expr| или "expr"

};


struct throw_endless_stream_error {};

class EndlessStream {
public:
	EndlessStream(CoreInputStreamInt& stream);
	~EndlessStream();
	char read();
	operator bool()const;
	void unread_last();
private:
    CoreInputStreamInt& t_base;
    std::string t_buffer;
	std::string_view t_view_buf;
	size_t t_pos;
};

struct throw_token_stream_error {};
struct throw_token_stream_empty {};


struct EndlessStreamHelper {

	EndlessStreamHelper(EndlessStream& s);
	char get();
	void next();
	void unread_last();
	operator bool() const;
private:
	EndlessStream& t_base;
	char t_now = 0;
	bool t_readed = false;
};


class TokenStream {
public:
	TokenStream(bool skip_comments, EndlessStream& stream);
	std::pair<tokens, std::string> read();
private:

	std::pair<tokens, std::string> t_read(EndlessStreamHelper& s);
	std::string read_expr_under_DoubleQuote(EndlessStreamHelper& s);
	std::string read_expr_under_VerticalLine(EndlessStreamHelper& s);

	std::pair<tokens, std::string> read_break_symbol(EndlessStreamHelper& s);
	bool is_break_symbol(char symbol) const;
	bool is_skip_symbol(char symbol) const;
private:
	EndlessStream& t_base;
	bool t_skip_comments;
};

enum class read_reason { success, stream_fail, wrong_input, empty_input };

class Syntaxer
{
public:
	Syntaxer(Symbols& symbols, bool skip_comments = true, bool upcase_mode = true);

	void set_skip_comments_mode(bool mode);
	bool skip_comments_mode() const;

	void set_upcase_mode(bool mode);
	bool upcase_mode() const;

	std::pair<read_reason, char> read_char(CoreInputStreamInt& stream);
	std::pair<read_reason, Cell> read_sexpr(CoreInputStreamInt& stream);
	std::pair<read_reason, std::vector<Cell>> read_sexprs(CoreInputStreamInt& stream);
private:
	void process_char(char& symbol);
	void process_symbol(std::string& symbol);
	std::pair<read_reason, Cell> gen_cell(TokenStream& tokens);
	std::pair<read_reason, Cell> gen_cell(tokens token, std::string&& val , TokenStream& tokens);
private:
	std::reference_wrapper<Symbols> t_symbols;
	bool t_skip_comments;
	bool t_upcase_mode;
};
