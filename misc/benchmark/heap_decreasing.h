//
// Created by schrodinger on 19-5-18.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_HEAP_DECREASING_H
#define DATA_STRUCTURE_FOR_LOVE_HEAP_DECREASING_H

#include "benchmark.h"
#include <binary_heap.hpp>
#include <binomial_heap.hpp>
#include <fib_heap.hpp>
#include <pairing_heap.hpp>


namespace benchmark {
    using namespace data_structure;
#ifndef HEAP_RUNNER
#define HEAP_RUNNER

    struct HeapRunner : public BenchMark {
        std::string name;

        explicit HeapRunner(std::string name) noexcept : name(std::move(name)), BenchMark() {}
    };

#endif


    template<class Heap>
    struct HeapDecreasingRunner : public HeapRunner {


        explicit HeapDecreasingRunner(std::string name) noexcept : HeapRunner(std::move(name)) {}

        long long run(size_t n) {
            Heap heap;
            std::vector<int> vec;
            std::vector<typename Heap::value_holder> holders;
            gen_random_int(vec, n);
            holders.reserve(vec.size());
            for (auto i : vec) {
                holders.push_back(heap.push_and_hold(i));
            }
            auto a = time_now();
            for (auto i : holders) {
                i.re_push(i.get() - rand() % i.get());
            }
            auto b = time_now();
            return (b - a).count();
        }

        Result run() override {
            Result result;
            result.name = name;
            for (auto i = 1; i <= 10000; ++i) {
                auto t = run(i);
                result.outcomes.emplace_back(i, t);
            }
            return result;
        }
    };

    HeapDecreasingRunner<FibHeap<int>> fib_heap_dec("FibHeapDecreasing");
    HeapDecreasingRunner<BinomialHeap<int>> binomial_heap_dec("BinomialHeapDecreasing");
    HeapDecreasingRunner<PairingHeap<int>> pairing_heap_dec("PairingHeapDecreasing");
}
#endif //DATA_STRUCTURE_FOR_LOVE_HEAP_DECREASING_H
