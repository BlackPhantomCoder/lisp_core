#pragma once
#include <stack>
#include <vector>
#include <future>
#include "boost/pool/pool_alloc.hpp"

template<class T>
class MPool {
public:
    MPool(size_t new_block_size = 1024) :
        t_new_block_size(new_block_size),
        t_block(nullptr),
        t_block_size(0)
    {

    }
    template<class... Args>
    T* construct(Args&&... args) {
        T* result = get();
        *result = T(std::forward<Args&&>(args)...);
        return result;
    }

    T* get_default() {
        return get();
    }
    void free(T* val) {
        t_mem.push(val);
    }

    ~MPool() {
        while (!std::empty(t_blocks)) {
            delete[] t_blocks.top();
            t_blocks.pop();
        }
    }
private:
    T* get() {
        T* result = nullptr;
        if (t_block_size) {
            result = t_block;
            ++t_block;
            --t_block_size;
        }
        else if (!t_mem.empty()) {
            result = t_mem.top();
            t_mem.pop();
        }
        else {
            t_block = new T[t_new_block_size];
            t_block_size = t_new_block_size - 1;
            t_blocks.push(t_block);
            result = t_block;
            ++t_block;
        }
        return result;
    }
private:
    std::stack<T*, std::vector<T*>> t_blocks;
    std::stack<T*, std::vector<T*>> t_mem;
    T* t_block;
    size_t t_block_size;
    size_t t_new_block_size;
};

template<class T>
class MPoolWMutex : private MPool <T>{
public:
    MPoolWMutex(size_t new_block_size = 1024) :
        MPool<T>(new_block_size)
    {

    }
    template<class... Args>
    T* construct(Args&&... args) {
        std::lock_guard g(t_m);
        return MPool<T>::construct(args);
    }

    T* get_default() {
        std::lock_guard g(t_m);
        return MPool<T>::get_default();
    }
    void free(T* val) {
        std::lock_guard g(t_m);
        MPool<T>::free(val);
    }
private:
    std::mutex t_m;
};