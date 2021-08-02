#pragma once
#include "CoreStreamInt.h"
#include <iostream>
#include <vector>

class StdCoreInputMemoryStream : public CoreInputStreamInt {
public:
	StdCoreInputMemoryStream(std::istream& input, CoreData::stream_read_mode mode);
    StdCoreInputMemoryStream(const StdCoreInputMemoryStream& rh);
	virtual bool alive() const;
	virtual bool ready() const override;


	virtual CoreData::stream_read_mode get_mode() const override;
	void rewind();

protected:
    virtual std::string t_read_line() override;
private:
	std::vector<std::string> t_lines;
	std::vector<std::string>::const_iterator t_pos;
	CoreData::stream_read_mode t_mode;
};

