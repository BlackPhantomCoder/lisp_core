#pragma once
#include <string>
#include "CoreData.h"

class CoreInputStreamInt
{
public:
	virtual ~CoreInputStreamInt() = default;
	bool alive() const;

	
	//std::string get_line();
	virtual CoreData::stream_read_mode get_mode() const = 0;

	// -1 == eos
	int peek_char();
	// -1 == eos
	int get_char();
	void unread_last();
	bool is_unreaded() const;
protected:
	// -1 == eos
	virtual int t_read_char() = 0;
	// -1 == eos
	virtual int t_peek_char() = 0;
	virtual bool t_alive() const = 0;
private:
	int t_last;
	bool t_unreaded;
};

class CoreOutputStreamInt
{
public:
	virtual ~CoreOutputStreamInt() = default;
	virtual void out_new_line(const std::string& val) = 0;
	virtual void out_without_new_line(const std::string& val) = 0;
};