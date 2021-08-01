#pragma once
#include <string>
#include <iostream>
#include "CoreData.h"

class CoreInputStreamInt
{
public:
	virtual ~CoreInputStreamInt() = default;
	virtual bool alive() const = 0;
	virtual bool ready() const = 0;

	virtual char get_char() = 0;
	virtual std::string get_line() = 0;
	virtual  CoreData::stream_read_mode get_mode() const = 0;
	virtual void unread_some(long val) = 0;
};

class CoreOutputStreamInt
{
public:
	virtual ~CoreOutputStreamInt() = default;
	virtual void out_new_line(const std::string& val) = 0;
	virtual void out_without_new_line(const std::string& val) = 0;
};