//
// Created by schrodinger on 2/20/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_OBJECT_POOL_HPP
#define DATA_STRUCTURE_FOR_LOVE_OBJECT_POOL_HPP

#include <cstddef>
#include <vector>
#include <memory>
#include <cstring>
#include <bitset>
#include <object_management.hpp>
#include <optimized_vector.hpp>
#include <cassert>
#include <unordered_set>

namespace data_structure {
    template<typename T, std::size_t ChunkSize = 1000,
            typename PtrContainer = std::vector<T *>>
    class ObjectPool {

    public:
    public:
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef const T &const_reference;
        typedef T value_type;

        template<typename U>
        struct rebind {
            typedef ObjectPool<U> other;
        };

#if __cplusplus >= 201103L
        typedef std::true_type propagate_on_container_move_assignment;

        typedef std::true_type is_always_equal;
#endif

        ObjectPool() = default;

        ObjectPool(ObjectPool const &that) = delete;

        ObjectPool &operator=(ObjectPool const &that) = delete;

        void absorb(ObjectPool &that);

        ObjectPool(ObjectPool &&that) noexcept;

        ObjectPool &operator=(ObjectPool &&that) noexcept;

        [[nodiscard]] T *get_raw();

        [[nodiscard]] T *allocate(size_type t);

        template<typename ...Args>
        [[nodiscard]] T *construct_raw(Args &&...args);;

        template<typename ...Args>
        void construct(T *p, Args &&...args);

        inline void recycle(T *address);

        inline void destroy(T *address);

        inline void deallocate(T *address, size_type n);

        std::shared_ptr<T> get_shared();

        template<typename ...Args>
        std::shared_ptr<T> construct_shared(Args &&...args);;

        ~ObjectPool();

    private:
        T *chunk_end = nullptr, *current_address = nullptr;
        PtrContainer pool{};
        PtrContainer recycle_list{};

        void alloc_chunk();
    };

    template<typename T, size_t ChunkSize, typename PtrContainer>
    void ObjectPool<T, ChunkSize, PtrContainer>::absorb(ObjectPool &that) {
        while (current_address != chunk_end) {
            recycle_list.push_back(current_address++);
        }
        for (auto i: that.recycle_list) {
            recycle_list.push_back(i);
        }
        for (auto i: that.pool) {
            pool.push_back(i);
        }
        current_address = that.current_address;
        chunk_end = that.chunk_end;
        that.chunk_end = that.current_address = nullptr;
        that.pool.clear();
        that.recycle_list.clear();
    }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    ObjectPool<T, ChunkSize, PtrContainer>::ObjectPool(ObjectPool &&that) noexcept {
        chunk_end = that.chunk_end;
        current_address = that.current_address;
        that.current_address = that.chunk_end = nullptr;
        pool = std::move(that.pool);
        recycle_list = std::move(that.recycle_list);
    }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    ObjectPool<T, ChunkSize, PtrContainer> &
    ObjectPool<T, ChunkSize, PtrContainer>::operator=(ObjectPool &&that) noexcept {
        chunk_end = that.chunk_end;
        current_address = that.current_address;
        that.current_address = that.chunk_end = nullptr;
        pool = std::move(that.pool);
        recycle_list = std::move(that.recycle_list);
    }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    T *ObjectPool<T, ChunkSize, PtrContainer>::get_raw() {
        if (!recycle_list.empty()) {
            T *res = recycle_list.back();
            recycle_list.pop_back();
            return res;
        }
        if (chunk_end == current_address) alloc_chunk();
        return current_address++;
    }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    T *ObjectPool<T, ChunkSize, PtrContainer>::allocate(ObjectPool::size_type t) {
        if (chunk_end == current_address && t <= ChunkSize) {
            alloc_chunk();
        }
        if (t < chunk_end - current_address) {
            current_address += t;
            return current_address - t;
        } else {
            pool.push_back(static_cast<T *>(::operator new(sizeof(T) * t)));
            return pool.back();
        }
    }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    template<typename... Args>
    T *ObjectPool<T, ChunkSize, PtrContainer>::construct_raw(Args &&... args) {
        T *address = get_raw();
        utils::emplace_construct(address, std::forward<Args>(args)...);
        return address;
    }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    template<typename... Args>
    void ObjectPool<T, ChunkSize, PtrContainer>::construct(T *p, Args &&... args) {
        return utils::emplace_construct(p, std::forward<Args>(args)...);
    }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    void ObjectPool<T, ChunkSize, PtrContainer>::recycle(T *address) {
        std::memset(address, 0, sizeof(T));
        recycle_list.push_back(address);
    }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    void ObjectPool<T, ChunkSize, PtrContainer>::destroy(T *address) { utils::destroy_at(address); }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    void ObjectPool<T, ChunkSize, PtrContainer>::deallocate(T *address, ObjectPool::size_type n) {
        while (n--)
            recycle(address++);
    }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    std::shared_ptr<T> ObjectPool<T, ChunkSize, PtrContainer>::get_shared() {
        return {get_raw(), [u = this](T *t) { u->recycle(t); }};
    }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    template<typename... Args>
    std::shared_ptr<T> ObjectPool<T, ChunkSize, PtrContainer>::construct_shared(Args &&... args) {
        auto temp = construct_raw(std::forward<Args>(args)...);
        return std::shared_ptr<T>(temp, [u = this](T *t) { u->recycle(t); });
    }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    ObjectPool<T, ChunkSize, PtrContainer>::~ObjectPool() {
        for (auto i : pool) {
            ::operator delete(static_cast<void *>(i), sizeof(T) * ChunkSize);
        }
    }

    template<typename T, size_t ChunkSize, typename PtrContainer>
    void ObjectPool<T, ChunkSize, PtrContainer>::alloc_chunk() {
        pool.push_back(static_cast<T *>(::operator new(sizeof(T) * ChunkSize)));
        chunk_end = current_address = pool.back();
        chunk_end += ChunkSize;
    }

    template<class T, class U>
    bool operator==(const ObjectPool<T> &, const ObjectPool<U> &) { return true; }

    template<class T, class U>
    bool operator!=(const ObjectPool<T> &, const ObjectPool<U> &) { return false; }

}

#endif //DATA_STRUCTURE_FOR_LOVE_OBJECT_POOL_HPP
