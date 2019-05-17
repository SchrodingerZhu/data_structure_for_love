//
// Created by schrodinger on 19-5-18.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_ALLOCATION_H
#define DATA_STRUCTURE_FOR_LOVE_ALLOCATION_H

#include <object_pool.hpp>

namespace benchmark {
    using namespace data_structure;

    struct SLTAllocator : public BenchMark {
        explicit SLTAllocator() noexcept : BenchMark() {}

        long long run(size_t n) {
            int *vec[n];
            std::allocator<int> alloc;
            auto a = time_now();
            for (int i = 0; i < n; ++i) {
                vec[i] = alloc.allocate(1);
                alloc.construct(vec[i], rand());
            }
            for (int i = 0; i < n; ++i) {
                alloc.destroy(vec[i]);
                alloc.deallocate(vec[i], 1);
            }
            auto b = time_now();
            return (b - a).count();
        }

        Result run() override {
            Result result;
            result.name = "STLAlloc";
            for (auto i = 1; i <= 10000; ++i) {
                auto t = run(i);
                result.outcomes.emplace_back(i, t);
            }
            return result;
        }
    } stl_alloc;

    struct ObjPoolAlloc : public BenchMark {
        explicit ObjPoolAlloc() noexcept : BenchMark() {}

        long long run(size_t n) {
            int *vec[n];
            ObjectPool<int, 10000> alloc;
            auto a = time_now();
            for (int i = 0; i < n; ++i) {
                vec[i] = alloc.allocate(1);
                alloc.construct(vec[i], rand());
            }
            for (int i = 0; i < n; ++i) {
                alloc.destroy(vec[i]);
                alloc.deallocate(vec[i], 1);
            }
            auto b = time_now();
            return (b - a).count();
        }

        Result run() override {
            Result result;
            result.name = "ObjPoolAlloc";
            for (auto i = 1; i <= 10000; ++i) {
                auto t = run(i);
                result.outcomes.emplace_back(i, t);
            }
            return result;
        }
    } obj_pool_alloc;


}
#endif //DATA_STRUCTURE_FOR_LOVE_ALLOCATION_H
