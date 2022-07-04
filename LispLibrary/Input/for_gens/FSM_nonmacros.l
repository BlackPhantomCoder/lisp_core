comment_line ;[^\n]*(\n|\z)
str1     \"([^\"\\]|\\\\|\\\"|\\[^\"])*\"
str2     \|([^\|\\]|\\\\|\\\"|\\[^\"])*\|
%%
{comment_line}				{}//# comment
{str1}                      {}//# str1
{str2}                      {}//# str2
%%
