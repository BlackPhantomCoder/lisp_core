#pragma once
#include <future>
#include <mutex>

template<class T>
class Mutexted {
public:
    Mutexted(const T& val = {}) : t_val(val) {}
    Mutexted(T&& val) : t_val(std::move(val)) {}
    const T& get() const { return t_val; }
    void set(const T& new_val) { std::lock_guard g(t_m); t_val = new_val; }
    void set(T&& new_val) { std::lock_guard g(t_m); t_val = std::move(new_val); }

private:
    std::mutex t_m;
    T t_val;
};