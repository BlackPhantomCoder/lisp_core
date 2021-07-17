#include "Cell.h"
#include "Funcs.h"
#include "Core.h"
#include "profile.h"
#include "PredLispFuncs.h"
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "TestsList.h"
#include "STDMutexed.h"

void cin_execute_kostil_repl(istream& is) {
    Core core = make_core_w_predfuncs(cin, cout);
    if (is) {

        for (const auto& exp : read_expressions_from_stream(is, core, stream_read_mode::s_expression, true)) {
            Core::result_type result_reason = Core::result_type::success;
            string result;
            {
                //LogDuration a;
                auto [result_reason2, result2] = core.execute(exp);
                result = move(result2);
                result_reason = result_reason2;
            }
            cout << result_reason << ", " << result << endl;
        }
    }

    //auto [result_reason2, result2] = core.execute("(loop (print (eval (progn (print '>) (read)))))");
    while (cin)
    {
        cout << "> ";
        auto [readed, exp] = read_one_expression_from_stream(cin, core, stream_read_mode::new_string, true);
        
        if (!readed) {
            cout << "wrong input" << endl;
            continue;
        }
        Core::result_type result_reason = Core::result_type::success;
        string result;
        {
            LogDuration a;
            auto [result_reason2, result2] = core.execute(exp);
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
    cin_execute_kostil_repl(f);

    //запуск репла с cin + предзагрузка из потока (в данном случае - stringstream) 
    //cin_execute_kostil_repl(s);
}