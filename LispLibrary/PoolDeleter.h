#pragma once

template<class T>
struct PollObjDeleter {
	typedef void (*delptr) (T*);
public:
	PollObjDeleter(delptr ptr) : t_fnc(ptr) {}
	PollObjDeleter(const PollObjDeleter&) = default;
	PollObjDeleter(PollObjDeleter&&) = default;
	void operator()(T* p) const {
		if (t_fnc)t_fnc(p);
	};

	PollObjDeleter& operator=(PollObjDeleter&& rh) = default;
	PollObjDeleter& operator=(const PollObjDeleter& rh) = default;
private:
	delptr t_fnc;
};