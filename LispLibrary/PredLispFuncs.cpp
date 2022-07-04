#include "PredLispFuncs.h"
#include "StdCoreInputStream.h"
#include "StdCoreOutputStream.h"

#include "StdCoreInputMemoryStream.h"
#include <fstream>
#include <sstream>
using namespace std;
using namespace CoreData;

Core make_core()
{
    return Core(nullopt, nullopt);
}

Core make_core_w_custom_predfuncs(CoreInputStreamInt& predfunc)
{
    ostringstream s;
    auto out = StdCoreOutputStream(s);
    auto p = RefCESP(predfunc, out);
    return Core(p);
}

Core make_core_w_custom_predfuncs(CoreInputStreamInt& predfunc, std::unique_ptr<IMutexed<bool>>&& custom_mutex)
{
    ostringstream s;
    auto out = StdCoreOutputStream(s);
    auto p = RefCESP(predfunc, out);
    return Core(p, move(custom_mutex));
}

Core make_core_w_predfuncs()
{
    ifstream s(predfuncs_filename);
    StdCoreInputStream sc(s, stream_read_mode::s_expression);
    return make_core_w_custom_predfuncs(sc);
}

Core make_core_w_predfuncs(std::unique_ptr<IMutexed<bool>>&& custom_mutex)
{
    ifstream s(predfuncs_filename);
    StdCoreInputStream sc(s, stream_read_mode::s_expression);
    return make_core_w_custom_predfuncs(sc, move(custom_mutex));
}

Core make_core_from_state(std::istream& state_stream)
{
    return Core(state_stream);
}

Core make_core_from_state(std::istream& state_stream, std::unique_ptr<IMutexed<bool>>&& custom_mutex)
{
    return Core(state_stream, move(custom_mutex));
}