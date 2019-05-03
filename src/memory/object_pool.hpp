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
#include <iostream>
#include <cassert>
#include <unordered_set>
namespace data_structure {
    template<typename T, std::size_t ChunkSize = 1000,
            typename PtrContainer = std::vector<T *>>
    class ObjectPool {
    public:
        using size_type = std::size_t;
        using differece_type = std::ptrdiff_t;

        ObjectPool() = default;
        ObjectPool (ObjectPool const &that) = delete;

        ObjectPool &operator=(ObjectPool const &that) = delete;

        void absorb(ObjectPool& that) {
            while(current_address != chunk_end) {
                recycle_list.push_back(current_address++);
            }
            for(auto i: that.recycle_list) {
                recycle_list.push_back(i);
            }
            for(auto i: that.pool) {
                pool.push_back(i);
            }
            current_address = that.current_address;
            chunk_end = that.chunk_end;
            that.chunk_end = that.current_address = nullptr;
            that.pool.clear();
            that.recycle_list.clear();
        }
        ObjectPool(ObjectPool &&that) noexcept {
            chunk_end = that.chunk_end;
            current_address = that.current_address;
            that.current_address = that.chunk_end =  nullptr;
            pool = std::move(that.pool);
            recycle_list = std::move(that.recycle_list);
        }

        ObjectPool &operator=(ObjectPool &&that) noexcept {
            chunk_end = that.chunk_end;
            current_address = that.current_address;
            that.current_address = that.chunk_end =  nullptr;
            pool = std::move(that.pool);
            recycle_list = std::move(that.recycle_list);
        }

        [[nodiscard]] T *get_raw() {
            if (!recycle_list.empty()) {
                T *res = recycle_list.back();
                recycle_list.pop_back();
                return res;
            }
            if (chunk_end == current_address) alloc_chunk();
            return current_address++;
        }

        [[nodiscard]] T *allocate(size_type t) {
            if(chunk_end == current_address && t <= ChunkSize) {
                alloc_chunk();
            }
            if (t < chunk_end - current_address)  {
                current_address += t;
                return current_address - t;
            } else {
                pool.push_back(static_cast<T *>(::operator new(sizeof(T) * t)));
                return pool.back();
            }
        }
    
        template <typename ...Args>
        [[nodiscard]] T *construct_raw(Args&& ...args) {
            T* address = get_raw();
            utils::emplace_construct(address, std::forward<Args>(args)...);
            return address;
        };

        template <typename ...Args>
        void construct(T *p, Args&& ...args)
        {return utils::emplace_construct(p, std::forward<Args>(args)...);}

        inline void recycle(T* address) {
            std::memset(address, 0, sizeof(T));
            recycle_list.push_back(address);
        }

        inline void destroy(T * address) { utils::destroy_at(address); }
        inline void deallocate(T * address, size_type n) {while(n--) recycle(address++); }
        std::shared_ptr<T> get_shared() {
            return {get_raw(), [u = this] (T* t) {u->recycle(t);}};
        }

        template <typename ...Args>
        std::shared_ptr<T> construct_shared(Args&& ...args) {
            auto temp = construct_raw(std::forward<Args>(args)...);
            return std::shared_ptr<T>(temp, [u = this] (T* t) { u->recycle(t);});
        };

        ~ObjectPool() {
            for(auto i : pool) {
                ::operator delete(static_cast<void *>(i), sizeof(T) * ChunkSize);
            }
        }
    private:
        T *chunk_end = nullptr, *current_address = nullptr;
        PtrContainer pool{};
        PtrContainer recycle_list {};
        void alloc_chunk() {
            pool.push_back(static_cast<T *>(::operator new(sizeof(T) * ChunkSize)));
            chunk_end = current_address = pool.back();
            chunk_end += ChunkSize;
        }
    };

}

#endif //DATA_STRUCTURE_FOR_LOVE_OBJECT_POOL_HPP
