#include "Cell.h"
#include "Funcs.h"
#include "Core.h"
#include "profile.h"
#include "PredLispFuncs.h"
#include "StdCoreInputStream.h"
#include "StdCoreInputMemoryStream.h"
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
using namespace CoreData;

#include "TestsList.h"
#include "STDMutexed.h"

void cin_execute_kostil_repl(CoreInputStreamInt& is) {
    Core core = make_core_w_predfuncs(cin, stream_read_mode::new_string,  cout);
    if (is.ready()) 
    {
        core.execute_all(is);
    }

    //auto [result_reason2, result2] = core.execute("(loop (print (eval (progn (print '>) (read)))))");
    StdCoreInputStream s(cin, stream_read_mode::new_string);
    while (cin)
    {
        cout << "> ";
        Core::result_type result_reason = Core::result_type::success;
        string result;
        {
            string str;
            getline(cin, str);
            LogDuration a;
            auto [result_reason2, result2] = core.execute_one(str);
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
    auto cf = StdCoreInputStream(f, stream_read_mode::s_expression);
    cin_execute_kostil_repl(cf);

    //запуск репла с cin + предзагрузка из потока (в данном случае - stringstream) 
    //cin_execute_kostil_repl(s);
}