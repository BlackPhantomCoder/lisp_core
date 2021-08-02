#pragma once
#include <string>
#include "CoreData.h"

class CoreInputStreamInt
{
public:
	virtual ~CoreInputStreamInt() = default;
	virtual bool alive() const = 0;
	virtual bool ready() const = 0;

	std::string get_line();
	virtual CoreData::stream_read_mode get_mode() const = 0;

	void unread_last_line();
	void erase_atlast(size_t val);
	const std::string& last_line();
protected:
	virtual std::string t_read_line() = 0;
	void t_clear_last();
private:
	std::string t_last;
	bool t_unreaded;
};

class CoreOutputStreamInt
{
public:
	virtual ~CoreOutputStreamInt() = default;
	virtual void out_new_line(const std::string& val) = 0;
	virtual void out_without_new_line(const std::string& val) = 0;
};