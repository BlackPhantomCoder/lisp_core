#pragma once
#include "StdCoreInputArrayInput.h"
#include "CoreStreamInt.h"

#include <iostream>
#include <vector>

class StdCoreInputMemoryStream : public StdCoreInputValArrayInput<std::vector<std::string>> {
public:
	StdCoreInputMemoryStream(std::istream& input, CoreData::stream_read_mode mode);
};