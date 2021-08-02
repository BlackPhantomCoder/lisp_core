#include "PredLispFuncs.h"
#include "StdCoreInputStream.h"
#include "StdCoreOutputStream.h"

#include "StdCoreInputMemoryStream.h"
#include <fstream>
#include <sstream>
using namespace std;
using namespace CoreData;

Core make_core_w_custom_predfuncs(CoreInputStreamInt& predfunc, std::istream& in, CoreData::stream_read_mode mode, std::ostream& out)
{
    return Core(
        predfunc,
        make_unique<CoreEnvStreamsProvider>(
            make_unique<StdCoreInputStream>(in, mode), make_unique<StdCoreOutputStream>(out)
            )
    );
}

std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_custom_predfuncs_and_empty_streams(CoreInputStreamInt& predfunc)
{
    auto strms = std::make_unique<empty_streams>();
    return
        std::pair<Core, std::unique_ptr<empty_streams>>{
        std::piecewise_construct_t(),
            std::forward_as_tuple(
                predfunc,
                make_unique<CoreEnvStreamsProvider>(
                    make_unique<StdCoreInputStream>(strms->in, stream_read_mode::new_string), make_unique<StdCoreOutputStream>(strms->out)
                    )
            ),
            std::forward_as_tuple(move(strms))
    };
}

Core make_core_w_custom_predfuncs(CoreInputStreamInt& predfunc, std::istream& in, std::ostream& out, std::unique_ptr<IMutexed<bool>>&& custom_mutex)
{
    return Core(
        predfunc,
        make_unique<CoreEnvStreamsProvider>(
            make_unique<StdCoreInputStream>(in, stream_read_mode::new_string), make_unique<StdCoreOutputStream>(out)
            ),
        move(custom_mutex)
    );
}

std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_custom_predfuncs_and_empty_streams(CoreInputStreamInt& predfunc, std::unique_ptr<IMutexed<bool>>&& custom_mutex)
{
    auto strms = std::make_unique<empty_streams>();
    return
        std::pair<Core, std::unique_ptr<empty_streams>>{
        std::piecewise_construct_t(),
            std::forward_as_tuple(
                predfunc,
                make_unique<CoreEnvStreamsProvider>(
                    make_unique<StdCoreInputStream>(strms->in, stream_read_mode::new_string), make_unique<StdCoreOutputStream>(strms->out)
                    ),
                move(custom_mutex)
            ),
            std::forward_as_tuple(move(strms))
    };
}

Core make_core_w_predfuncs(std::istream& in, stream_read_mode mode, std::ostream& out)
{
    ifstream s (predfuncs_filename);
    StdCoreInputStream sc(s, stream_read_mode::s_expression);
    return make_core_w_custom_predfuncs(sc, in, mode, out);
}

std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_predfuncs_and_empty_streams()
{
    ifstream s(predfuncs_filename);
    StdCoreInputStream sc(s, stream_read_mode::s_expression);
    return make_core_w_custom_predfuncs_and_empty_streams(sc);
}

Core make_core_w_predfuncs(std::istream& in, std::ostream& out, std::unique_ptr<IMutexed<bool>>&& custom_mutex)
{
    ifstream s(predfuncs_filename);
    StdCoreInputStream sc(s, stream_read_mode::s_expression);
    return make_core_w_custom_predfuncs(sc,in, out, move(custom_mutex));
}

std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_predfuncs_and_empty_streams(std::unique_ptr<IMutexed<bool>>&& custom_mutex)
{
    ifstream s(predfuncs_filename);
    StdCoreInputStream sc(s, stream_read_mode::s_expression);
    return make_core_w_custom_predfuncs_and_empty_streams(sc, move(custom_mutex));
}
