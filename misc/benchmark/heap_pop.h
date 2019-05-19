//
// Created by schrodinger on 19-5-18.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_HEAP_POP_H
#define DATA_STRUCTURE_FOR_LOVE_HEAP_POP_H

#include "benchmark.h"
#include <binary_heap.hpp>
#include <binomial_heap.hpp>
#include <fib_heap.hpp>
#include <pairing_heap.hpp>
#include <queue>


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
    struct HeapPopRunner : public HeapRunner {


        explicit HeapPopRunner(std::string name) noexcept : HeapRunner(std::move(name)) {}

        long long run(size_t n) {
            Heap heap;
            std::vector<int> vec;
            gen_random_int(vec, n);
            for (auto i : vec) {
                heap.push(i);
            }
            auto a = time_now();
            while (!heap.empty()) {
                heap.pop();
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

    HeapPopRunner<BinaryHeap<int>> binary_heap_pop("BinaryHeapPop");
    HeapPopRunner<FibHeap<int>> fib_heap_pop("FibHeapPop");
    HeapPopRunner<BinomialHeap<int>> binomial_heap_pop("BinomialHeapPop");
    HeapPopRunner<PairingHeap<int>> pairing_heap_pop("PairingHeapPop");
    HeapPopRunner<std::priority_queue<int>> pq_pop("PQPop");
}
#endif //DATA_STRUCTURE_FOR_LOVE_HEAP_POP_H
