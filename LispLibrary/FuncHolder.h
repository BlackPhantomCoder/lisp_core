#pragma once

class Func;
class FuncHolder {
public:
	FuncHolder(Func* ptr);
	FuncHolder(FuncHolder&& rh) noexcept;
	FuncHolder(const FuncHolder& rh) = delete;

	~FuncHolder();

	FuncHolder& operator=(FuncHolder&& rh)noexcept;
	FuncHolder& operator=(const FuncHolder& rh) = delete;

	Func& operator*() const;
	Func* operator->() const;
	Func* get()const;
private:
	Func* t_func = nullptr;
};
