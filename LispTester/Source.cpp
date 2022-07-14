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

void save_load_state() {
    auto s_saved = stringstream();
    {
        ofstream f("programs/s.json");
        auto core = make_core_w_predfuncs();
        core.save_state(f);
    }
}

void cin_execute_kostil_repl(CoreInputStreamInt& is) {
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

#include "Func.h"
#include "RangeBifunc.h"
#include "RangeNBifunc.h"
#include "Bifuncs.h"
#include "SupportFuncs.h"
#include <locale>
int main()
{
    //для ввода...
    system("chcp 1251");

    //установка локали
    setlocale(LC_ALL, "ru-RU");
    locale::global(locale("ru-RU"));
    //запуск тестов (в эту функцию писать все запуски тестов)
    run_all_tests();
    //запуск репла с cin + предзагрузка из потока (в данном случае - файл) 
    ifstream f("programs/1.lsp");
    auto cf = StdCoreInputStream(f, stream_read_mode::s_expression);
    cin_execute_kostil_repl(cf);

    //save_load_state();
}