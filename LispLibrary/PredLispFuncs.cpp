#include "PredLispFuncs.h"
#include <fstream>
using namespace std;

Core make_core_w_predfuncs(std::istream& in, std::ostream& out)
{
    ifstream f(predfuncs_filename);
    return Core(f, in, out);
}

std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_predfuncs_and_empty_streams()
{
    auto s = std::make_unique<empty_streams>();
    ifstream f(predfuncs_filename);
    return 
        std::pair<Core, std::unique_ptr<empty_streams>>{ 
            std::piecewise_construct_t(),
            std::forward_as_tuple(f, s->in, s->out),
            std::forward_as_tuple(move(s)) 
        };
}