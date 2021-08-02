#pragma once
#include <iostream>
#include "CoreStreamInt.h"

class StdCoreOutputStream : public CoreOutputStreamInt {
public:
	StdCoreOutputStream(std::ostream& output);

	virtual void out_new_line(const std::string& val) override;
	virtual void out_without_new_line(const std::string& val) override;

private:
	std::reference_wrapper<std::ostream> t_stream;
};