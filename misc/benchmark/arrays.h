//
// Created by schrodinger on 19-5-18.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_ARRAYS_H
#define DATA_STRUCTURE_FOR_LOVE_ARRAYS_H

#include <optimized_vector.hpp>
#include <rootish_stack.hpp>

namespace benchmark {
    using namespace data_structure;

    struct SLTVector : public BenchMark {
        explicit SLTVector() noexcept : BenchMark() {}

        long long run(size_t n) {
            std::vector<int> vec, test;
            gen_random_int(vec, n);
            auto a = time_now();
            for (auto i : vec) {
                test.push_back(i);
            }
            for (int i = 0; i < vec.size(); ++i) {
                test[i] = rand();
            }
            auto b = time_now();
            return (b - a).count();
        }

        Result run() override {
            Result result;
            result.name = "STLVector";
            for (auto i = 1; i <= 10000; ++i) {
                auto t = run(i);
                result.outcomes.emplace_back(i, t);
            }
            return result;
        }
    } stl_vector;

    struct OptArray : public BenchMark {
        explicit OptArray() noexcept : BenchMark() {}

        long long run(size_t n) {
            std::vector<int> vec;
            optimized_vector<int, 10000> test;
            gen_random_int(vec, n);
            auto a = time_now();
            for (auto i : vec) {
                test.push_back(i);
            }
            for (int i = 0; i < vec.size(); ++i) {
                test[i] = rand();
            }
            auto b = time_now();
            return (b - a).count();
        }

        Result run() override {
            Result result;
            result.name = "OptArray";
            for (auto i = 1; i <= 10000; ++i) {
                auto t = run(i);
                result.outcomes.emplace_back(i, t);
            }
            return result;
        }
    } opt_array;

    struct Rootish : public BenchMark {
        explicit Rootish() noexcept : BenchMark() {}

        long long run(size_t n) {
            std::vector<int> vec;
            RootishStack<int> test;
            gen_random_int(vec, n);
            auto a = time_now();
            for (auto i : vec) {
                test.push_back(i);
            }
            for (int i = 0; i < vec.size(); ++i) {
                test[i] = rand();
            }
            auto b = time_now();
            return (b - a).count();
        }

        Result run() override {
            Result result;
            result.name = "RootishArray";
            for (auto i = 1; i <= 10000; ++i) {
                auto t = run(i);
                result.outcomes.emplace_back(i, t);
            }
            return result;
        }
    } Rootish;

}
#endif //DATA_STRUCTURE_FOR_LOVE_ARRAYS_H
