#include "Cell.h"
#include "Funcs.h"
#include "BIFuncCell.h"
#include "OnceCore.h"
#include "ReplCore.h"
#include <string>
#include <list>
#include <vector>
#include <iostream>

using namespace std;

void execute_once(std::string input) {
    OnceCore core(std::move(input));

    unsigned long i = 0;
    while (!core.is_finished()) {
        cout << "do something " << i++ << endl;
       /* if (i == 25) {
            core.forse_stop();
        }*/
    }
    auto [result_reason, result] = core.result();

    switch (result_reason)
    {
    case OnceCore::result_type::fail: cout << "fail";
        break;
    case OnceCore::result_type::success: cout << "success";
        break;
    case OnceCore::result_type::forsed_stopped: cout << "forsed_stopped";
        break;
    }

    cout << ", " << result << endl;
}

void execute_repl(istream& is) {
    ReplCore core;
    while (is) {
        string input;
        cout << "> ";
        std::getline(is, input);
        core.execute(input);
        unsigned long i = 0;
        while (!core.is_finished()) {
            cout << "do something " << i++ << endl;
        }
        auto [result_reason, result] = core.last_result();

        switch (result_reason)
        {
        case ReplCore::result_type::fail: cout << "fail";
            break;
        case ReplCore::result_type::success: cout << "success";
            break;
        case ReplCore::result_type::forsed_stopped: cout << "forsed_stopped";
            break;
        }

        cout << ", " << result << endl;
    }
}

int main()
{
    //init_lisp();
    //repl("> ");

    //cout << std::numeric_limits<double>::max() << endl;
    /*std::string input;
    std::getline(cin, input);
    input =  
        "(progn "
            "(defun fib3 (n)"
                "(cond"
                "((<= n 0) nil)"
                "((= n 1) 1)"
                "((= n 2) 2)"
                "(T (fib3_rec 1 2 (- n 2)))"
                ")"
            ")" 
            "(defun fib3_rec (one two n)"
                "(if (= n 0)"
                "two"
                "(fib3_rec two (+ one two) (- n 1))"
                ")"
            ")"
            + 
            input 
            + 
        ")";*/
    execute_repl(cin);
}