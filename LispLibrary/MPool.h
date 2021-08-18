#pragma once
#include <stack>
#include <vector>
#include <future>
#include <queue>
#include <memory>
#include <algorithm>
#include <iostream>
#include "boost/pool/pool_alloc.hpp"

template <class T, size_t new_block_size>
class FrameArray;

template <class T, size_t new_block_size>
class FrameArrayIterator {
    friend class FrameArray<T, new_block_size>;
public:

    using value_type = T*;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

public:
    FrameArrayIterator(const FrameArrayIterator& rh) :
        t_arr(rh.t_arr),
        t_offset(rh.t_offset)
    {

    }
    FrameArrayIterator& operator=(const FrameArrayIterator& rh) {
        if (this != &rh) {
            t_arr = rh.t_arr;
            t_offset = rh.t_offset;
        }
        return *this;
    }

    FrameArrayIterator(FrameArray<T, new_block_size>& arr, size_t offset) :
        t_arr(&arr),
        t_offset(offset)
    {

    }

    FrameArrayIterator operator+(size_t i)const {
        FrameArrayIterator res = *this;
        res.t_offset += i;
        return res;
    }

    FrameArrayIterator operator-(size_t i) const {
        FrameArrayIterator res = *this;
        res.t_offset -= i;
        return res;
    }

    difference_type operator-(FrameArrayIterator i) const {
        return  t_offset - i.t_offset;
    }

    FrameArrayIterator& operator+=(difference_type i) {
        t_offset += i;
        return *this;
    }

    FrameArrayIterator operator++(int) {
        FrameArrayIterator res = *this;
        ++t_offset;
        return res;
    }

    FrameArrayIterator& operator++() {
        ++t_offset;
        return *this;
    }

    FrameArrayIterator operator--(int) {
        FrameArrayIterator res = *this;
        --t_offset;
        return res;
    }

    FrameArrayIterator& operator--() {
        --t_offset;
        return *this;
    }

    reference operator*();
    const reference operator*()const;

    bool operator!=(const FrameArrayIterator& rh) const {
        return t_offset != rh.t_offset;
    }
    bool operator==(const FrameArrayIterator& rh) const {
        return t_offset == rh.t_offset;
    }
    bool operator<(const FrameArrayIterator& rh) const {
        return t_offset < rh.t_offset;
    }
    bool operator>(const FrameArrayIterator& rh) const {
        return t_offset > rh.t_offset;
    }
    bool operator<=(const FrameArrayIterator& rh) const {
        return t_offset <= rh.t_offset;
    }
    bool operator>=(const FrameArrayIterator& rh) const {
        return t_offset >= rh.t_offset;
    }
private:
    FrameArray<T, new_block_size>* t_arr;
    size_t t_offset;
};

template <class T, size_t new_block_size>
class FrameArray {
    using frame = std::array<T*, new_block_size>;
    using iterator = FrameArrayIterator<T, new_block_size>;
public:

    void add() {
        t_frames.push_back({});
    }
    T* const& at(size_t pos) const {
        return t_frames.at(pos / new_block_size)[pos % new_block_size];
    }
    T*& at(size_t pos) {
        return t_frames.at(pos / new_block_size)[pos % new_block_size];
    }
    void delete_block(iterator pos_it) {
        size_t block = pos_it.t_offset / new_block_size;
        size_t pos = pos_it.t_offset % new_block_size;
        if (pos == 0) {
            t_frames.erase(std::begin(t_frames) + block);
        }
        else {
            while (block >= 1) {
                swap_ranges(
                    std::begin(t_frames.at(block - 1)) + pos,
                    std::end(t_frames.at(block - 1)),
                    std::begin(t_frames.at(block))
                );
                swap_ranges(
                    std::begin(t_frames.at(block)),
                    std::begin(t_frames.at(block)) + pos,
                    std::end(t_frames.at(block + 1)) - pos
                );
                --block;
            }

            copy(
                std::begin(t_frames.at(0)),
                std::begin(t_frames.at(0)) + pos,
                std::begin(t_frames.at(1))
            );

            t_frames.erase(std::begin(t_frames));
        }
    }

    size_t size() const {
        return t_frames.size() * new_block_size;
    }
    void clear() {
        t_frames.clear();
    }

    iterator begin() {
        return { *this, 0 };
    }
    iterator end() {
        return { *this, this->size() };
    }

    void shrink_to_fit() {
        t_frames.shrink_to_fit();
    }
private:
    std::vector<frame> t_frames;
};



template <class T, size_t new_block_size = 16384>
class MPool {
public:
    MPool()
    {
        t_mem_last_i = 0;
        t_buffer = nullptr;
    }

    template<class... Args>
    T* construct(Args&&... args) {
        T* result = t_get();
        *result = T(std::forward<Args&&>(args)...);
        return result;
    }

    T* get_default() {
        return  t_get();
    }

    void free(T* val) {
        t_mem.at(++t_mem_last_i - 1) = val;
    }

    void clear_free() {
        using namespace std;
        if (t_blocks.empty()) return;

        if (t_mem_last_i == 0) return;
        if (t_mem_last_i == t_mem.size() && !t_buffer) {
            t_blocks.clear();
            t_blocks.shrink_to_fit();
            t_mem.clear();
            t_mem.shrink_to_fit();
            t_mem_last_i = 0;
            return;
        }

        auto* buf_ptr = (t_buffer) ? t_blocks.back().get() : nullptr;

        auto mem_end = begin(t_mem) + t_mem_last_i;
        sort(begin(t_mem), mem_end);
        sort(begin(t_blocks), end(t_blocks), [](const unique_ptr<T[]>& lh, const unique_ptr<T[]>& rh) { return lh.get() < rh.get(); });

        auto block_it = begin(t_blocks);
        auto it = begin(t_mem);
        auto find_func = [this, &block_it, &it, &mem_end]() mutable {
            if (block_it == end(t_blocks)) return;
            it = find(begin(t_mem), mem_end, block_it->get());
            while (
                it == mem_end
                ||
                (distance(it, mem_end) < new_block_size)
                ||
                (*(it + (new_block_size - 1)) != *it + (new_block_size - 1))
                ) {
                ++block_it;
                if (block_it == end(t_blocks)) return;
                it = find(begin(t_mem), mem_end, block_it->get());
            }
        };
        find_func();
        while (block_it != end(t_blocks) && it != mem_end) {
            t_mem.delete_block(it);
            t_mem_last_i -= new_block_size;
            mem_end = begin(t_mem) + t_mem_last_i;
            it = begin(t_mem);

            block_it = t_blocks.erase(block_it);

            find_func();
        }
        t_blocks.shrink_to_fit();
        t_mem.shrink_to_fit();

        if (buf_ptr && t_blocks.back().get() != buf_ptr) {
            if (t_blocks.size() > 1) {
                find_if(
                    begin(t_blocks),
                    end(t_blocks),
                    [buf_ptr](const std::unique_ptr<T[]>& rh) {return rh.get() == buf_ptr; }
                )->swap(*prev(end(t_blocks)));
            }
        }
    }

    ~MPool() {

    }
private:
    T* t_get() {
        if (t_buffer) {
            auto temp = t_buffer--;
            if (temp == t_blocks.back().get()) {
                t_buffer = nullptr;
            }
            return temp;
        }
        else if (t_mem_last_i != 0) {
            auto temp = t_mem.at(t_mem_last_i - 1);
            --t_mem_last_i;
            return temp;
        }
        else {
            t_blocks.push_back(std::make_unique<T[]>(new_block_size));
            t_mem.add();
            t_buffer = t_blocks.back().get() + (new_block_size - 1);
            return t_buffer--;
        }
    }
private:
    std::vector<std::unique_ptr<T[]>> t_blocks;
    FrameArray<T, new_block_size> t_mem;
    size_t t_mem_last_i;
    T* t_buffer;
};


template <class T, size_t new_block_size>
typename FrameArrayIterator<T, new_block_size>::reference FrameArrayIterator<T, new_block_size>::operator*()
{
    return t_arr->at(t_offset);
}

template <class T, size_t new_block_size>
typename const FrameArrayIterator<T, new_block_size>::reference  FrameArrayIterator<T, new_block_size>::operator*() const
{
    return t_arr->at(t_offset);
}



//16384
template<class T, size_t new_block_size = 16384>
class MPoolWMutex : private MPool <T, new_block_size>{
public:
    MPoolWMutex()
    {

    }
    template<class... Args>
    T* construct(Args&&... args) {
        std::lock_guard g(t_m);
        return MPool<T, new_block_size>::construct(args);
    }

    T* get_default() {
        std::lock_guard g(t_m);
        return MPool<T, new_block_size>::get_default();
    }
    void free(T* val) {
        std::lock_guard g(t_m);
        MPool<T, new_block_size>::free(val);
    }
    void clear_free() {
        std::lock_guard g(t_m);
        MPool<T, new_block_size>::clear_free();
    }
private:
    std::mutex t_m;
};