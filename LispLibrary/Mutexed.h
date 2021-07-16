#pragma once
template<class T>
class IMutexed {
public:
    virtual ~IMutexed() = default;
    virtual const T& get() const = 0;
    virtual void set(const T& new_val) = 0;
    virtual void set(T&& new_val) = 0;
};