#pragma once
#include <iostream>
#include "CoreStreamInt.h"



class StdCoreInputStream : public CoreInputStreamInt {
public:
	StdCoreInputStream(const StdCoreInputStream&) = delete;
	StdCoreInputStream(std::istream& input, CoreData::stream_read_mode mode);
	virtual bool alive() const;
	virtual bool ready() const override;


	virtual CoreData::stream_read_mode get_mode() const override;

	StdCoreInputStream& operator=(const StdCoreInputStream&) = delete;
protected:
	virtual std::string t_read_line() override;
private:
	std::reference_wrapper<std::istream> t_stream;
	CoreData::stream_read_mode t_mode;
};
