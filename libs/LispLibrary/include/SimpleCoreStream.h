#pragma once
#include "CoreStreamInt.h"

class SimpleInputCoreStream : public CoreInputStreamInt {
public:
	SimpleInputCoreStream(std::istream& input, CoreData::stream_read_mode mode);
	virtual bool alive() const;
	virtual bool ready() const override;

	virtual char get_char() override;
	virtual std::string get_line() override;
	virtual CoreData::stream_read_mode get_mode() const override;
	virtual void unread_some(long val) override;

private:
	std::reference_wrapper<std::istream> t_stream;
	CoreData::stream_read_mode t_mode;
};

class SimpleOutputCoreStream : public CoreOutputStreamInt {
public:
	SimpleOutputCoreStream(std::ostream& output);

	virtual void out_new_line(const std::string& val) override;
	virtual void out_without_new_line(const std::string& val) override;

private:
	std::reference_wrapper<std::ostream> t_stream;
};