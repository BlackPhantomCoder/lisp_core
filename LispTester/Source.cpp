#include "Cell.h"
#include "Funcs.h"
#include "OnceCore.h"
#include "ReplCore.h"
#include "SerialReplCore.h"
#include "profile.h"
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#include "TestsList.h"

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
    cin.sync_with_stdio(false);
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

void execute_serial_repl(istream& is) {
    cin.sync_with_stdio(false);
    SerialReplCore core;
    while (is) {
        cout << "> ";
        for (const auto& exp : listp_expressions_from_stream(is)) {
            SerialReplCore::result_type result_reason = SerialReplCore::result_type::success;
            string result;
            {
                LogDuration a;
                auto [result_reason2, result2] = core.execute(exp);
                result = move(result2);
                result_reason = result_reason2;
            }
            cout << result_reason << ", " << result << endl;
            cout << "> ";
        }
    }
}

void cin_execute_serial_repl(istream& is) {
    cin.sync_with_stdio(false);
    SerialReplCore core;
    if (is) {
        cout << "> ";
        for (const auto& exp : listp_expressions_from_stream(is, true)) {
            SerialReplCore::result_type result_reason = SerialReplCore::result_type::success;
            string result;
            {
                LogDuration a;
                auto [result_reason2, result2] = core.execute(exp);
                result = move(result2);
                result_reason = result_reason2;
            }
            cout << result_reason << ", " << result << endl;
            cout << "> ";
        }
    }
    while (cin) {
        cout << "> ";
        std::string input;
        getline(cin, input);
        SerialReplCore::result_type result_reason = SerialReplCore::result_type::success;
        string result;
        {
            LogDuration a;
            auto [result_reason2, result2] = core.execute(input);
            result = move(result2);
            result_reason = result_reason2;
        }
        cout << result_reason << ", " << result << endl;
    }
}

int main()
{

    //запуск тестов (в эту функцию писать все запуски тестов)
    run_all_tests();

    //stringstream s(
    //    "(defun fib3 (n)"
    //        "(cond"
    //        "((<= n 0) nil)"
    //        "((= n 1) 1)"
    //        "((= n 2) 2)"
    //        "(T (fib3_rec 1 2 (- n 2)))"
    //        ")"
    //    ")"
    //    "(defun fib3_rec (one two n)"
    //        "(if (= n 0)"
    //        "two"
    //        "(fib3_rec two (+ one two) (- n 1))"
    //        ")"
    //    ")"
    //    "(defun fib (n) (cond ((= n 1) 1) ((= n 2) 2) (T (+ (fib(- n 2)) (fib(- n 1))))))"
    //);


    
    //запуск репла с cin + предзагрузка из потока (в данном случае - файл) 
    ifstream f("programs/1.lsp");
    cin_execute_serial_repl(f);

    //запуск репла с cin + предзагрузка из потока (в данном случае - stringstream) 
    //cin_execute_serial_repl(s);
}