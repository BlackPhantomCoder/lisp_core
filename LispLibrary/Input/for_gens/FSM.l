delim   [ \t\n\r]
ws      {delim}+
symbol	  [^0-9 \"\|\\ \t\n\r\(\);\'\]-]|\\(.|\n)
numb	  -?[0-9]
numb_or_symbol -
nonmacros     ;|\"|\|
%%
{ws}                        {}//# witespace
{nonmacros}				    {}//# non_macros
{numb_or_symbol}			{}//# numb_or_symbol
{numb}                      {}//# numb
{symbol}                    {}//# symbol
%%
