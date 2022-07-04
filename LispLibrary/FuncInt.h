#pragma once
#include "CarCdrIterator.h"
#include "Cell.h"
#include "CoreData.h"

#include <memory>
#include <optional>

enum class eval_types { eval, no_eval };
enum class spread_types { spread, nospread };


class FuncInt
{
public:
	virtual ~FuncInt() = default;

	virtual bool execute() = 0;

	//need_external
	virtual void push_next(Cell&& result) = 0;
	virtual CoreData::HolderPtr get_next() = 0;

	//executed
	virtual Cell result() = 0;

};



class FuncHolder {
public:
	virtual ~FuncHolder() = default;
	virtual FuncInt& operator()() = 0;
};

template<class T>
class TFuncHolder : public FuncHolder {
public:
	TFuncHolder() = default;

	//template<class... Args>
	//TFuncHolder(Args&&... args) : t_data(T(std::forward<Args&&>(args)...)) {}

	template<class... Args>
	void init(Args&&... args) {
		t_data = T(std::forward<Args&&>(args)...);
	}

	virtual FuncInt& operator()() override {
		if (!t_data) throw "";
		return *t_data;
	}
	void empty() const {
		return !t_data;
	}
	void clear() {
		t_data.reset();
	}
private:
	std::optional<T> t_data;
};


template<class T>
void del_TFuncHolder(FuncHolder* ptr) {
	auto* p = (TFuncHolder<T>*)(ptr);
	p->clear();
	CoreData::get_pool<TFuncHolder<T>>().free(p);
}

template<class T, class... Args>
CoreData::HolderPtr make_fnc(Args&&... args) {
	auto* obj_ptr = CoreData::get_pool<TFuncHolder<T>>().get_default();
	obj_ptr->init(std::forward<Args&&>(args)...);
	return std::unique_ptr<TFuncHolder<T>, PollObjDeleter<FuncHolder>>(obj_ptr, PollObjDeleter<FuncHolder>(&del_TFuncHolder<T>));
}