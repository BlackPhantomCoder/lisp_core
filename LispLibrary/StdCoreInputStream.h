#pragma once
#include <iostream>
#include "CoreStreamInt.h"



class StdCoreInputStream : public CoreInputStreamInt {
public:
	StdCoreInputStream(const StdCoreInputStream&) = delete;
	StdCoreInputStream(std::istream& input, CoreData::stream_read_mode mode);

	virtual CoreData::stream_read_mode get_mode() const override;

	StdCoreInputStream& operator=(const StdCoreInputStream&) = delete;
protected:
	virtual int t_read_char() override;
	virtual int t_peek_char() override;
	virtual bool t_alive() const override;
private:
	std::reference_wrapper<std::istream> t_stream;
	CoreData::stream_read_mode t_mode;
	//char t_buf;
	bool t_eos = false;
};
