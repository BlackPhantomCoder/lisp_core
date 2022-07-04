#include "parser.h"
#include "Input/parser.tab.h"
namespace tokens {
	const int YYEOF = yytokentype::YYEOF;
	const int str = yytokentype::str;
	const int symbol = yytokentype::symbol;
	const int z_numb = yytokentype::z_numb;
	const int r_numb = yytokentype::r_numb;
	const int Macro = yytokentype::Macro;
	const int FinalMacro = yytokentype::FinalMacro;

	const int YYPUSH_MORE = ::YYPUSH_MORE;
}