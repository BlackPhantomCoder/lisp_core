#pragma once
#include "Mutexed.h"
#include <future>
#include <mutex>

template<class T>
class STDMutexed : public IMutexed<T> {
public:
    STDMutexed(const T& val = {}) : t_val(val) {}
    STDMutexed(T&& val) : t_val(std::move(val)) {}
    virtual const T& get() const override { return t_val; }
    virtual void set(const T& new_val) override { std::lock_guard g(t_m); t_val = new_val; }
    virtual void set(T&& new_val) override { std::lock_guard g(t_m); t_val = std::move(new_val); }

private:
    std::mutex t_m;
    T t_val;
};