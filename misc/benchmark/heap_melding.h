//
// Created by schrodinger on 19-5-18.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_HEAP_MELDING_H
#define DATA_STRUCTURE_FOR_LOVE_HEAP_MELDING_H

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
    struct HeapMeldingRunner : public HeapRunner {


        explicit HeapMeldingRunner(std::string name) noexcept : HeapRunner(std::move(name)) {}

        long long run(size_t n) {
            Heap heap0, heap1;
            std::vector<int> vec0;
            std::vector<int> vec1;
            gen_random_int(vec0, n);
            gen_random_int(vec1, n);
            for (auto i : vec0) {
                heap0.push(i);
            }
            for (auto i : vec0) {
                heap1.push(i);
            }
            auto a = time_now();
            heap0.merge(heap1);
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

    HeapMeldingRunner<FibHeap<int>> fib_heap_meld("FibHeapMelding");
    HeapMeldingRunner<BinomialHeap<int>> binomial_heap_meld("BinomialHeapMelding");
    HeapMeldingRunner<PairingHeap<int>> pairing_heap_meld("PairingHeapMelding");
}
#endif //DATA_STRUCTURE_FOR_LOVE_HEAP_MELDING_H
