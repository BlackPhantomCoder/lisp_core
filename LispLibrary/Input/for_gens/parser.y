%{
#include <stdio.h>
#include <stdlib.h>
    int yyerror(void* ph, const char* s);

extern  unsigned long make_empty_sp(void* ph);
extern  unsigned long make_sp(void* ph, unsigned long base, unsigned long elem);
extern  unsigned long make_list(void* ph, unsigned long sp);
extern  unsigned long make_dp(void* ph, unsigned long f, unsigned long s);
extern  void save(void* ph, unsigned long cell);
extern  unsigned long make_symbol(void* ph, const char* str);
extern  void output_sp(void* ph, const char*, unsigned long sp);
extern  void output_cell(void* ph, const char*, unsigned long cell);
extern  void saved_to_final(void* ph);

extern  void save_symbol_or_macro(void* ph, char ch);
extern  void save_if_or_macro(void* ph, char ch, int* result);
%}
%parse-param { void* ph }
%define parse.error verbose

%code requires {
#define YYSTYPE unsigned long
}
%define api.pure full
%define api.push-pull push

%token str
%token symbol
%token z_numb
%token r_numb
%token Macro
%token FinalMacro

%token '(';
%token ')';
%token '.';
%token '\'';

%%
START     : S                           { save(ph, $1); YYACCEPT; }
            |
            S_final                     { save(ph, $1); saved_to_final(ph); YYACCEPT; }
            |
            '.'                         { save_symbol_or_macro(ph, '.'); YYACCEPT;  }
            |
            ']'                         { int res = 0; save_if_or_macro(ph, ']', &res); if(res == 1) YYACCEPT; else YYABORT;  }
            |
            ','                         { int res = 0; save_if_or_macro(ph, ']', &res); if(res == 1) YYACCEPT; else YYABORT;  }
S         : A | Dp | L | Q | FinalMacro { $$ = $1; output_cell(ph, "[FinalMacro]", $1); }
            |
            Macro                       { $$ = $1; output_cell(ph, "[Macro]", $1); }

A         : str | symbol | N;
N         : z_numb | r_numb;
Dp        : '(' Sp '.' S ')'            { $$ = make_dp(ph, $2, $4); output_cell(ph, "[Dp]", $$); };
Sp        : S Sp                        { $$ = make_sp(ph, $2, $1); output_sp(ph, "[Sp]", $$); };
            |
            S                           { $$ = make_sp(ph, make_empty_sp(ph), $1); output_sp(ph, "[Sp1]", $$); };
            ;
L         : '(' Sp ')'                  { $$ = make_list(ph, $2); output_cell(ph, "[L]", $$); }
            |
            '(' ')'                     { $$ = make_list(ph, make_empty_sp(ph)); output_cell(ph, "[empty L]", $$); }
Q         : '\'' S                      { $$ = make_list(ph, make_sp(ph, make_sp(ph, make_empty_sp(ph), $2), make_symbol(ph, "QUOTE"))); output_cell(ph, "[Q]", $$); }



S_final   : L_final | Q_final;
L_final   : '(' Sp ']'                  { $$ = make_list(ph, $2); output_cell(ph, "[Lfinal]", $$); }
            |
            '(' Sp_final                { $$ = make_list(ph, $2); output_cell(ph, "[Lfinal L]", $$); }
Sp_final  : S Sp_final                  { $$ = make_sp(ph, $2, $1); output_sp(ph, "[Sp_final]", $$); };
            |
            S_final                     { $$ = make_sp(ph, make_empty_sp(ph), $1); output_sp(ph, "[Sp_final1]", $$); };
            ;
Q_final   : '\'' S_final                { $$ = make_list(ph, make_sp(ph, make_sp(ph, make_empty_sp(ph), $2), make_symbol(ph, "QUOTE"))); output_cell(ph, "[Q]", $$); }

%%

int yyerror(void* ph, const char* s)
{
    return 0;
}