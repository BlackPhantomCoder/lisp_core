#pragma once
#include "CoreStreamInt.h"
#include "SExprsFarm.h"

#include <vector>
#include <string>
#include <stack>
#include <string>

enum class tokens {
	OpenBracket,	// (
	CloseBracket,	// )
	Expr,			// выражение
	Quote,			// '
	//DoubleQuote,	// "
	//BackQuote,		// `
	//Comma,			// ,
	//CommercialAt,   // @
	Dot,			// .
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

	EndlessStreamHelper(CoreInputStreamInt& s);
	char get();
	void next();
	void unread_last();
	operator bool() const;
private:
	EndlessStream t_base;
	char t_now = 0;
	bool t_readed;
};


class TokenStream {
public:
	TokenStream(bool skip_comments, CoreInputStreamInt& stream);
	std::pair<tokens, std::string> read();

	bool alive() const;
	bool ready() const;
private:
	std::string t_read_expr_under_DoubleQuote();
	std::string t_read_expr_under_VerticalLine();

	std::pair<tokens, std::string> t_read_break_symbol();
	bool t_is_break_symbol(char symbol) const;
	bool t_is_skip_symbol(char symbol) const;
	bool t_is_ignore_symbol(char symbol) const;
private:
	enum class check_next_reason {skip_s, break_s, ignore_s, empty_stream, another_s};
private:
	check_next_reason t_check_next(bool skip_ignorse);
private:
	EndlessStreamHelper t_base;
	CoreInputStreamInt& t_stream;
	bool t_skip_comments;
};

enum class read_reason { success, stream_fail, wrong_input, empty_input };


struct throw_SExprStreamError {};
class Syntaxer;

class SExprStream {
public:
	SExprStream(Syntaxer& owner, CoreInputStreamInt& stream);
	std::pair<read_reason, Cell> read();
	bool alive() const;
	bool ready() const;

	
private:
	TokenStream t_base;
	Syntaxer& t_owner;
};




class Syntaxer
{
	friend class SExprStream;
public:
	Syntaxer(SExprsFarm& ferm, bool skip_comments = true, bool upcase_mode = true);

	void set_skip_comments_mode(bool mode);
	bool skip_comments_mode() const;

	void set_upcase_mode(bool mode);
	bool upcase_mode() const;

	std::pair<read_reason, char> read_char(CoreInputStreamInt& stream);
	std::pair<read_reason, Cell> read_sexpr(CoreInputStreamInt& stream);
	std::pair<read_reason, std::vector<Cell>> read_sexprs(CoreInputStreamInt& stream);

	SExprStream read_sexprs_stream(CoreInputStreamInt& stream);
private:
	void process_char(char& symbol);
	void process_symbol(std::string& symbol);
	std::pair<read_reason, Cell> gen_cell(TokenStream& tokens);
	std::pair<read_reason, Cell> gen_cell(tokens token, std::string&& val , TokenStream& tokens);
private:
	std::reference_wrapper<SExprsFarm> t_farm;
	bool t_skip_comments;
	bool t_upcase_mode;
};

