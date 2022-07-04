#pragma once
#include "Core.h"
#include "CoreData.h"

#include <vector>

const static auto predfuncs_filename = "programs/predfuncs.lsp";

Core make_core();

Core make_core_w_custom_predfuncs(CoreInputStreamInt& predfunc);
Core make_core_w_custom_predfuncs(CoreInputStreamInt& predfunc, std::unique_ptr<IMutexed<bool>>&& custom_mutex);

Core make_core_w_predfuncs();
Core make_core_w_predfuncs(std::unique_ptr<IMutexed<bool>>&& custom_mutex);

Core make_core_from_state(std::istream& state_stream);
Core make_core_from_state(std::istream& state_stream, std::unique_ptr<IMutexed<bool>>&& custom_mutex);
