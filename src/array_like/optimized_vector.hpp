/*
 * Created by schrodinger on 2/16/19.
 * Reference: small_vector implement by ICEYSELF.
 * Reference link: https://dev.tencent.com/u/ICEYSELF/p/ckxc-v2/git/blob/master/include/sona/small_vector.hpp
 * ----------------------------------------------------------------------------------------------------------
 * data_structure::optimized_vector
 * A vector implement with optimization for small amount data.
 */

#ifndef DATA_STRUCTURE_FOR_LOVE_OPTIMIZED_VECTOR_HPP
#define DATA_STRUCTURE_FOR_LOVE_OPTIMIZED_VECTOR_HPP

#include <cstddef>
#include <cstring>
#include <memory>
#include <object_management.hpp>

#ifdef MEMORY_LEAK_TEST

#include <cassert>
#include <iostream>

#endif // MEMORY_LEAK_TEST
namespace data_structure {
    template<typename T, std::size_t LocalSize = 13>
    class optimized_vector {
    public:
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = T &;
        using pointer = T *;
        using iterator = T *;
        using const_iterator = T const *;

        optimized_vector() noexcept;

        explicit optimized_vector(size_type count, value_type const &value = value_type());

        optimized_vector(optimized_vector const &that);

        optimized_vector(optimized_vector &&that) noexcept;

        optimized_vector(std::initializer_list<T> init_list);

        ~optimized_vector();

        iterator begin() noexcept;

        iterator end() noexcept;

        inline const_iterator begin() const noexcept;

        inline const_iterator end() const noexcept { return cend(); }

        const_iterator cbegin() const;

        const_iterator cend() const;

        value_type &operator[](size_type n) { return *(begin() + n); }

        value_type const &operator[](size_type n) const { return *(cbegin() + n); }

        size_type size() const noexcept { return cend() - cbegin(); }

        size_type capacity() const noexcept;

        void push_back(value_type const &value);

        void push_back(value_type &&value);

        template<typename ...Args>
        void emplace_back(Args &&...args);

        void pop_back();

        void erase(size_type position);

        void clear();

        void resize(const size_t &target, value_type const &value = value_type());

        void empty_fill_local();

        bool empty() { return size() == 0; }

        T &back() { return *(end() - 1); }

    private:
#ifdef MEMORY_LEAK_TEST
        std::size_t ALLOCED = 0, FREED = 0;
#endif // MEMORY_LEAK_TEST

        inline void assure_capacity(const size_t &target) noexcept;

        inline void mark_size(const size_t &target) noexcept;

        enum class MEM_STATE : unsigned char {
            uninitialized,
            locally,
            on_heap
        } mem_state;
        union MEM_UNION {
            struct {
                std::aligned_storage_t<LocalSize * sizeof(T), alignof(T)> storage;
                size_type usage;
            } local_storage;
            struct {
                T *mem_start, *mem_end, *mem_usage;
            } heap_storage;
        } mem_union;
    };

    template<typename T, size_t LocalSize>
    optimized_vector<T, LocalSize>::optimized_vector() noexcept {
        static_assert(LocalSize, "LocalSize cannot be zero");
        std::memset(reinterpret_cast<void *>(&mem_union), 0, sizeof(mem_union));
        mem_state = MEM_STATE::uninitialized;
    }

    template<typename T, size_t LocalSize>
    optimized_vector<T, LocalSize>::optimized_vector(optimized_vector::size_type count, const value_type &value) {
        static_assert(LocalSize, "LocalSize cannot be zero");
        mem_state = MEM_STATE::uninitialized;
        std::memset(reinterpret_cast<void *>(&mem_union), 0, sizeof(mem_union));
        if (count == 0) return;
        else {
            assure_capacity(count);
            mark_size(count);
            for (size_type i = 0; i < count; ++i) {
                utils::emplace_construct<T>(begin() + i, value);
            }
        }
    }

    template<typename T, size_t LocalSize>
    optimized_vector<T, LocalSize>::optimized_vector(optimized_vector const &that) {
        static_assert(LocalSize, "LocalSize cannot be zero");
        mem_state = MEM_STATE::uninitialized;
        std::memset(reinterpret_cast<void *>(&mem_union), 0, sizeof(mem_union));
        if (that.mem_state == MEM_STATE::uninitialized) return;
        else {
            assure_capacity(that.size());
            mark_size(that.size());
            std::uninitialized_copy(that.begin(), that.end(), begin());
        }
    }

    template<typename T, size_t LocalSize>
    optimized_vector<T, LocalSize>::optimized_vector(optimized_vector &&that) noexcept {
        static_assert(LocalSize, "LocalSize cannot be zero");
        mem_state = that.mem_state;
        std::memset(reinterpret_cast<void *>(&mem_union), 0, sizeof(mem_union));
        if (that.mem_state == MEM_STATE::uninitialized) return;
        else if (that.mem_state == MEM_STATE::locally) {
            mark_size(that.size());
            std::uninitialized_move(that.begin(), that.end(), begin());
        } else {
            mem_union.heap_storage.mem_start = that.mem_union.heap_storage.mem_start;
            mem_union.heap_storage.mem_usage = that.mem_union.heap_storage.mem_usage;
            mem_union.heap_storage.mem_end = that.mem_union.heap_storage.mem_end;
#ifdef MEMORY_LEAK_TEST
            this->ALLOCED = that.ALLOCED;
                this->FREED = that.FREED;
                that.FREED = that.ALLOCED = 0;
#endif // MEMORY_LEAK_TEST
        }
        mem_state = that.mem_state;
        that.mem_state = MEM_STATE::uninitialized;
        std::memset(reinterpret_cast<void *>(&that.mem_union), 0, sizeof(that.mem_union));
    }

    template<typename T, size_t LocalSize>
    optimized_vector<T, LocalSize>::optimized_vector(std::initializer_list<T> init_list) {
        static_assert(LocalSize, "LocalSize cannot be zero");
        assure_capacity(init_list.size());
        mark_size(init_list.size());
        std::uninitialized_copy(init_list.begin(), init_list.end(), begin());
    }

    template<typename T, size_t LocalSize>
    optimized_vector<T, LocalSize>::~optimized_vector() {
        if (mem_state == MEM_STATE::uninitialized) return;
        for (auto &value : *this) utils::destroy_at(&value);
        if (mem_state == MEM_STATE::on_heap) {
            ::operator delete(reinterpret_cast<void * >(mem_union.heap_storage.mem_start));
#ifdef MEMORY_LEAK_TEST
            FREED += (mem_union.heap_storage.mem_end - mem_union.heap_storage.mem_start) * sizeof(T);
#endif // MEMORY_LEAK_TEST
        }
#ifdef MEMORY_LEAK_TEST
        assert(FREED == ALLOCED);
#endif // MEMORY_LEAK_TEST
    }

    template<typename T, size_t LocalSize>
    typename optimized_vector<T, LocalSize>::iterator optimized_vector<T, LocalSize>::begin() noexcept {
        if (mem_state == MEM_STATE::locally) {
            return reinterpret_cast<T *>(&(mem_union.local_storage.storage));
        } else if (mem_state == MEM_STATE::on_heap) {
            return mem_union.heap_storage.mem_start;
        } else {
            return nullptr;
        }
    }

    template<typename T, size_t LocalSize>
    typename optimized_vector<T, LocalSize>::iterator optimized_vector<T, LocalSize>::end() noexcept {
        if (mem_state == MEM_STATE::locally) {
            return reinterpret_cast<T *>(&(mem_union.local_storage.storage)) + mem_union.local_storage.usage;
        } else if (mem_state == MEM_STATE::on_heap) {
            return mem_union.heap_storage.mem_usage;
        } else {
            return nullptr;
        }
    }

    template<typename T, size_t LocalSize>
    typename optimized_vector<T, LocalSize>::const_iterator
    optimized_vector<T, LocalSize>::begin() const noexcept { return cbegin(); }

    template<typename T, size_t LocalSize>
    typename optimized_vector<T, LocalSize>::const_iterator optimized_vector<T, LocalSize>::cbegin() const {
        if (mem_state == MEM_STATE::locally) {
            return reinterpret_cast<T const *>(&(mem_union.local_storage.storage));
        } else if (mem_state == MEM_STATE::on_heap) {
            return mem_union.heap_storage.mem_start;
        } else {
            return nullptr;
        }
    }

    template<typename T, size_t LocalSize>
    typename optimized_vector<T, LocalSize>::const_iterator optimized_vector<T, LocalSize>::cend() const {
        if (mem_state == MEM_STATE::locally) {
            return reinterpret_cast<T const *>(&(mem_union.local_storage.storage)) + mem_union.local_storage.usage;
        } else if (mem_state == MEM_STATE::on_heap) {
            return mem_union.heap_storage.mem_usage;
        } else {
            return nullptr;
        }
    }

    template<typename T, size_t LocalSize>
    typename optimized_vector<T, LocalSize>::size_type optimized_vector<T, LocalSize>::capacity() const noexcept {
        if (mem_state == MEM_STATE::uninitialized) {
            return 0;
        } else if (mem_state == MEM_STATE::locally) {
            return LocalSize;
        } else {
            return mem_union.heap_storage.mem_end - mem_union.heap_storage.mem_start;
        }
    }

    template<typename T, size_t LocalSize>
    void optimized_vector<T, LocalSize>::push_back(const value_type &value) {
        assure_capacity(size() + 1);
        if (mem_state == MEM_STATE::locally) {
            utils::emplace_construct<T>(
                    reinterpret_cast<T *>(&(mem_union.local_storage.storage)) + (mem_union.local_storage.usage++),
                    value);
        } else {
            utils::emplace_construct<T>(mem_union.heap_storage.mem_usage++, value);
        }
    }

    template<typename T, size_t LocalSize>
    void optimized_vector<T, LocalSize>::push_back(value_type &&value) {
        assure_capacity(size() + 1);
        if (mem_state == MEM_STATE::locally) {
            utils::emplace_construct<T>(
                    reinterpret_cast<T *>(&(mem_union.local_storage.storage)) + (mem_union.local_storage.usage++),
                    std::move(value));
        } else {
            utils::emplace_construct<T>(mem_union.heap_storage.mem_usage++, std::move(value));
        }
    }

    template<typename T, size_t LocalSize>
    template<typename... Args>
    void optimized_vector<T, LocalSize>::emplace_back(Args &&... args) {
        assure_capacity(size() + 1);
        if (mem_state == MEM_STATE::locally) {
            utils::emplace_construct<T>(
                    reinterpret_cast<T *>(&(mem_union.local_storage.storage)) + (mem_union.local_storage.usage++),
                    std::forward<Args>(args)...);
        } else {
            utils::emplace_construct<T>(mem_union.heap_storage.mem_usage++, std::forward<Args>(args)...);
        }
    }

    template<typename T, size_t LocalSize>
    void optimized_vector<T, LocalSize>::pop_back() {
        mark_size(size() - 1);
        utils::destroy_at(end());
    }

    template<typename T, size_t LocalSize>
    void optimized_vector<T, LocalSize>::erase(optimized_vector::size_type position) {
        if (position >= size()) return;
        else {
            utils::destroy_at(begin() + position);
            std::uninitialized_move(begin() + position + 1, end(), begin() + position);
            mark_size(size() - 1);
        }
    }

    template<typename T, size_t LocalSize>
    void optimized_vector<T, LocalSize>::clear() { // TODO: Add test
        if (mem_state == MEM_STATE::uninitialized) return;
        for (auto &value : *this) utils::destroy_at(&value);
        if (mem_state == MEM_STATE::on_heap) {
            ::operator delete(reinterpret_cast<void * >(mem_union.heap_storage.mem_start));
#ifdef MEMORY_LEAK_TEST
            FREED += (mem_union.heap_storage.mem_end - mem_union.heap_storage.mem_start) * sizeof(T);
#endif // MEMORY_LEAK_TEST
        }
        std::memset(reinterpret_cast<void *>(&mem_union), 0, sizeof(mem_union));
        mem_state = MEM_STATE::uninitialized;
    }

    template<typename T, size_t LocalSize>
    void optimized_vector<T, LocalSize>::resize(const size_t &target, const value_type &value) { // TODO: Add test
        while (size() > target) pop_back();
        if (size() < target) {
            assure_capacity(target);
            mark_size(target);
            for (size_type i = 0; i < target; ++i) {
                utils::emplace_construct<T>(begin() + i, value);
            }
        }
    }

    template<typename T, size_t LocalSize>
    void optimized_vector<T, LocalSize>::empty_fill_local() {
        std::memset(reinterpret_cast<void *>(&mem_union), 0, sizeof(mem_union));
        mem_state = MEM_STATE::locally;
        mem_union.local_storage.usage = 0;
        mark_size(LocalSize);
    }

    template<typename T, size_t LocalSize>
    void optimized_vector<T, LocalSize>::assure_capacity(const size_t &target) noexcept {
        if (mem_state == MEM_STATE::uninitialized) {
            mem_state = MEM_STATE::locally;
            mem_union.local_storage.usage = 0;
        }
        if (capacity() >= target) return;
        if (mem_state == MEM_STATE::on_heap) {
            size_type new_capacity = capacity();
            while (new_capacity < target) new_capacity <<= 1u;
            auto *mem_start = reinterpret_cast<T *>(::operator new(new_capacity * sizeof(value_type)));
#ifdef MEMORY_LEAK_TEST
            ALLOCED += new_capacity * sizeof(value_type);
#endif // MEMORY_LEAK_TEST
            T *mem_end = mem_start + new_capacity;
            T *mem_usage = mem_start + size();
            std::uninitialized_move(begin(), end(), mem_start);
            for (auto iter = begin(), the_end = end(); iter < the_end; ++iter) {
                utils::destroy_at<T>(&(*iter));
            }
            ::operator delete(reinterpret_cast<void * >(mem_union.heap_storage.mem_start));
#ifdef MEMORY_LEAK_TEST
            FREED += (mem_union.heap_storage.mem_end - mem_union.heap_storage.mem_start) * sizeof(T);
#endif // MEMORY_LEAK_TEST
            mem_union.heap_storage.mem_start = mem_start;
            mem_union.heap_storage.mem_end = mem_end;
            mem_union.heap_storage.mem_usage = mem_usage;
        } else {
            size_type new_capacity = capacity();
            while (new_capacity < target) new_capacity <<= 1u;
            auto *mem_start = reinterpret_cast<T *>(::operator new(new_capacity * sizeof(value_type)));
#ifdef MEMORY_LEAK_TEST
            ALLOCED += new_capacity * sizeof(value_type);
#endif // MEMORY_LEAK_TEST
            T *mem_end = mem_start + new_capacity;
            T *mem_usage = mem_start + size();
            std::uninitialized_move(begin(), end(), mem_start);
            for (auto iter = begin(), the_end = end(); iter < the_end; ++iter) {
                utils::destroy_at<T>(&(*iter));
            }
            std::memset(reinterpret_cast<void *>(&mem_union), 0, sizeof(mem_union));
            mem_union.heap_storage.mem_start = mem_start;
            mem_union.heap_storage.mem_end = mem_end;
            mem_union.heap_storage.mem_usage = mem_usage;
            mem_state = MEM_STATE::on_heap;
        }
    }

    template<typename T, size_t LocalSize>
    void optimized_vector<T, LocalSize>::mark_size(const size_t &target) noexcept {
        if (capacity() < target) return; /// @attention should not be called
        else if (mem_state == MEM_STATE::locally) {
            mem_union.local_storage.usage = target;
        } else {
            mem_union.heap_storage.mem_usage = mem_union.heap_storage.mem_start + target;
        }
    }

}

#endif //DATA_STRUCTURE_FOR_LOVE_OPTIMIZED_VECTOR_HPP
