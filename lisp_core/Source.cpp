#include "Cell.h"
#include "Funcs.h"
#include "FuncCell.h"
#include "Interprintator.h"

#include <string>
#include <list>
#include <vector>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

// convert given string to list of tokens
std::list<std::string> tokenize(const std::string& str)
{
    std::list<std::string> tokens;
    const char* s = str.c_str();
    while (*s) {
        while (*s == ' ')
            ++s;
        if (*s == '(' || *s == ')')
            tokens.push_back(*s++ == '(' ? "(" : ")");
        else {
            const char* t = s;
            while (*t && *t != ' ' && *t != '(' && *t != ')') {
                ++t;
            }
            tokens.push_back(std::string(s, t));
            s = t;
        }
    }
    return tokens;
}

// return the Lisp expression in the given tokens
cell read_from(std::list<std::string>& tokens)
{
    std::string token(std::move(tokens.front()));
    tokens.pop_front();
    if (token == "(") {
        std::list<cell> lst;
        while (tokens.front() != ")")
            lst.push_back(read_from(tokens));
        tokens.pop_front();
        return make_list(std::move(lst));
    }
    else
        return make_atom(std::move(token));
}

// return the Lisp expression represented by the given string
cell read(const std::string& s) {
    std::list<std::string> tokens(tokenize(s));
    //FREERAM_PRINT_TEXT("before read_from: ");
    return read_from(tokens);
}

// convert given cell to a Lisp-readable string
std::string to_string(const cell& exp)
{
    if (exp.is_list()) {
        const auto& lst = exp.to_list();
        if (lst.empty()) return "nil";
        std::string s("(");
        for (auto e = std::begin(lst); e != std::end(lst); ++e){
            s += to_string(*e);
            s += ' ';
        }
            
        if (s[s.length() - 1] == ' '){
            s.erase(s.length() - 1);
        }
        s += ')';
        return s;
    }
    else if (is_symbol(exp.to_atom().c_str())) {
        return exp.to_atom();
    }
    else {
        return exp.to_atom();
    }
}
// the default read-eval-print-loop
void repl(const std::string& prompt)
{
    cell_env empty_env;
    cout << to_string(eval(read(
        
            "(defun fib3_rec (one two n)"
                "(if (= n 0)"
                    "two"
                    "(fib3_rec two (+ one two) (- n 1))"
                ")"
            ")"
        )
    , empty_env)).c_str() << endl;

    //1021
    //(f 1 1 7)
    //ev 241
    //"(defun f (o tt n) (if (= n 0) tt (f tt (+ o tt) (- n 1))))"

    cout << to_string(eval(read(
         
             "(defun fib3 (n)"
                 "(cond"
                     "((<= n 0) nil)"
                     "((= n 1) 1)"
                     "((= n 2) 2)"
                     "(T (fib3_rec 1 2 (- n 2)))"
                 ")"
             ")"
         )
     , empty_env)).c_str() << endl;

    //ev 98
    //"(+ 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36)"
   
    for (;;) {
        cout << prompt.c_str();
        cell c;
        {
            std::string str;
            getline(cin, str);
            c = read(str);
           
        }
        //FREERAM_PRINT_TEXT("before eval: "));
        cout << to_string(eval(c, empty_env)).c_str() << endl;
    }
}


int main()
{

    init_lisp();

    repl("> ");
}