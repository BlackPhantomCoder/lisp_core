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
    cin.get();
    auto core = make_core_w_predfuncs(cin, stream_read_mode::new_string, cout);
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
    //запуск репла с cin + предзагрузка из потока (в данном случае - файл) 
    ifstream f("programs/1.lsp");
    auto cf = StdCoreInputStream(f, stream_read_mode::s_expression);
    //for (size_t i = 0; i < 1000000; ++i) new long[480];
    cin_execute_kostil_repl(cf);

}