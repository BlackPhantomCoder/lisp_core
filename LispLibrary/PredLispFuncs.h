#pragma once
#include "Core.h"
#include <vector>
#include "CoreData.h"

const static auto predfuncs_filename = "programs/predfuncs.lsp";


Core make_core_w_custom_predfuncs(CoreInputStreamInt& predfunc, std::istream& in, CoreData::stream_read_mode mode, std::ostream& out);
std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_custom_predfuncs_and_empty_streams(CoreInputStreamInt& predfunc);

Core make_core_w_custom_predfuncs(CoreInputStreamInt& predfunc, std::istream& in, std::ostream& out, std::unique_ptr<IMutexed<bool>>&& custom_mutex);
std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_custom_predfuncs_and_empty_streams(CoreInputStreamInt& predfunc, std::unique_ptr<IMutexed<bool>>&& custom_mutex);


Core make_core_w_predfuncs(std::istream& in, CoreData::stream_read_mode mode, std::ostream& out);
std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_predfuncs_and_empty_streams();

Core make_core_w_predfuncs(std::istream& in, std::ostream& out, std::unique_ptr<IMutexed<bool>>&& custom_mutex);
std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_predfuncs_and_empty_streams(std::unique_ptr<IMutexed<bool>>&& custom_mutex);
