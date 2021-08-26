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
    auto core = make_core_w_predfuncs();
    {
        auto os = ostringstream();
        auto out = StdCoreOutputStream(os);
        auto streams = RefCESP(is, out);
        if (is.alive())
        {
            core.execute_all(streams);
        }
    }
    //auto is = istringstream("(loop (print (eval (progn (print '>) (read)))))");
    {
        auto s = StdCoreInputStream(cin, stream_read_mode::new_string);
        auto out = StdCoreOutputStream(cout);
        auto streams = RefCESP(s, out);
        auto b = core.execute_driver(streams);
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