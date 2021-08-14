#pragma once
#include "CarCdrIterator.h"
#include "Cell.h"

#include <memory>
enum class stages {executed, need_external, intermediate,  not_started};

enum class eval_types { eval, no_eval };
enum class spread_types { spread, nospread };

//class FuncInt;
//using fnc_data = std::unique_ptr<FuncInt>;


class FuncInt
{
public:
	virtual ~FuncInt() = default;

	virtual stages execute() = 0;
	virtual stages stage() const = 0;

	//need_external
	virtual void push_next(const Cell& result) = 0;
	virtual FuncInt& get_next() = 0;

	//executed
	virtual Cell result() const = 0;

};