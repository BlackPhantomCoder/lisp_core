#include "PredLispFuncs.h"
#include "SimpleCoreStream.h"
#include <fstream>
#include <sstream>
using namespace std;
using namespace CoreData;

Core make_core_w_predfuncs(std::istream& in, stream_read_mode mode, std::ostream& out)
{
    ifstream s (predfuncs_filename);
    SimpleInputCoreStream sc(s, stream_read_mode::s_expression);
    return Core(
        sc,
        make_unique<CoreEnvStreamsProvider>(
            make_unique<SimpleInputCoreStream>(in, mode), make_unique<SimpleOutputCoreStream>(out)
        )
    );
}

std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_predfuncs_and_empty_streams()
{
    auto strms = std::make_unique<empty_streams>();
    ifstream s(predfuncs_filename);
    SimpleInputCoreStream sc(s, stream_read_mode::s_expression);
    return 
        std::pair<Core, std::unique_ptr<empty_streams>>{ 
            std::piecewise_construct_t(),
            std::forward_as_tuple(
                sc,
                make_unique<CoreEnvStreamsProvider>(
                    make_unique<SimpleInputCoreStream>(strms->in, stream_read_mode::new_string), make_unique<SimpleOutputCoreStream>(strms->out)
                )
            ),
            std::forward_as_tuple(move(strms))
        };
}

Core make_core_w_predfuncs(std::istream& in, std::ostream& out, std::unique_ptr<IMutexed<bool>>&& custom_mutex)
{
    ifstream s(predfuncs_filename);
    SimpleInputCoreStream sc(s, stream_read_mode::s_expression);
    return Core(
        sc,
        make_unique<CoreEnvStreamsProvider>(
            make_unique<SimpleInputCoreStream>(in, stream_read_mode::new_string), make_unique<SimpleOutputCoreStream>(out)
        ),
        move(custom_mutex)
    );
}

std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_predfuncs_and_empty_streams(std::unique_ptr<IMutexed<bool>>&& custom_mutex)
{
    auto strms = std::make_unique<empty_streams>();
    ifstream s(predfuncs_filename);
    SimpleInputCoreStream sc(s, stream_read_mode::s_expression);
    return
        std::pair<Core, std::unique_ptr<empty_streams>>{
        std::piecewise_construct_t(),
            std::forward_as_tuple(
                sc,
                make_unique<CoreEnvStreamsProvider>(
                    make_unique<SimpleInputCoreStream>(strms->in, stream_read_mode::new_string), make_unique<SimpleOutputCoreStream>(strms->out)
                ),
                move(custom_mutex)
            ),
            std::forward_as_tuple(move(strms))
    };
}
