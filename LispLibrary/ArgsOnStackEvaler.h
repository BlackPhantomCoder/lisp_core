#pragma once
#include "LambdaCell.h"
#include "CellEnvironment.h"
#include "Cell.h"
#include <stack>

class CoreEnvironment;
template <class frame, class result>
class ArgsOnStackEvaler{
public:
	ArgsOnStackEvaler(CoreEnvironment* env) :
		t_env(env)
	{

	}

	void push(frame&& f) {
		t_frames.push(std::move(f));
	}

	virtual result pop_eval() = 0;

	void clear() {
		while (!t_frames.empty()) {
			t_frames.pop();
		}
	}
protected:
	CoreEnvironment* t_env;
	//vector invalidate references
	std::stack< frame, std::deque<frame>> t_frames;
};

