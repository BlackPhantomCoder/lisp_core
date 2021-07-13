#pragma once
#include "Core.h"

const static auto predfuncs_filename = "programs/predfuncs.lsp";

Core make_core_w_predfuncs(std::istream& in, std::ostream& out);
std::pair<Core, std::unique_ptr<empty_streams>> make_core_w_predfuncs_and_empty_streams();