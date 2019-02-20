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

namespace data_structure {
    template<typename T, std::size_t ChunkSize = 1000,
            typename PtrContainer = std::vector<T *>>
    class ObjectPool {
    private:
        using ObjPool = ObjectPool<T, ChunkSize, PtrContainer>;
        using Chunk = std::aligned_storage_t<ChunkSize * sizeof(T), alignof(T)>;
    public:
        using size_type = std::size_t;
        using differece_type = std::ptrdiff_t;

        ObjectPool() = default;
        ObjectPool (ObjPool const &that) = delete;

        ObjectPool &operator=(ObjPool const &that) = delete;

        ObjectPool(ObjPool &&that) noexcept {
            chunk_end = that.chunk_end;
            current_address = that.current_address;
            that.current_address = that.chunk_end =  nullptr;
            pool = std::move(that.pool);
            recycle_list = std::move(that.recycle_list);
        }

        ObjectPool &operator=(ObjPool &&that) noexcept {
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

        template <typename ...Args>
        [[nodiscard]] T *construct_raw(Args&& ...args) {
            T* address = get_raw();


            utils::emplace_construct(address, std::forward<Args>(args)...);
            return address;
        };

        inline void recycle(T* address) {
            utils::destroy_at(address);
            std::memset(address, 0, sizeof(T));
            recycle_list.push_back(address);
        }

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
                ::operator delete(reinterpret_cast<void *>(i), sizeof(T) * ChunkSize);
            }
        }
    private:
        T *chunk_end = nullptr, *current_address = nullptr;
        PtrContainer pool{};
        PtrContainer recycle_list {};
        void alloc_chunk() {
            pool.push_back(reinterpret_cast<T *>(::operator new(sizeof(T) * ChunkSize)));
            //std::memset(pool.back(), 0, sizeof(T) * ChunkSize);
            chunk_end = current_address = pool.back();
            chunk_end += ChunkSize;
        }
    };

}

#endif //DATA_STRUCTURE_FOR_LOVE_OBJECT_POOL_HPP
