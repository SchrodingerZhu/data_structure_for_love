//
// Created by schrodinger on 19-5-18.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_INTSET_ITERATION_H
#define DATA_STRUCTURE_FOR_LOVE_INTSET_ITERATION_H

#include "benchmark.h"
#include <van_emde_boas.hpp>
#include <x_fast_trie.hpp>
#include <y_fast_trie.hpp>
#include <binary_trie.hpp>
#include <algorithm>
#include <set>

namespace benchmark {
    using namespace data_structure;

#ifndef INTSET_RUNNER
#define INTSET_RUNNER

    struct IntsetRunner : public BenchMark {
        std::string name;

        explicit IntsetRunner(std::string name) noexcept : name(std::move(name)), BenchMark() {}
    };

#endif


    template<class IntSet>
    struct IntSetIterationRunner : public IntsetRunner {


        explicit IntSetIterationRunner(std::string name) noexcept : IntsetRunner(std::move(name)) {}

        long long run(size_t n) {
            IntSet tree;
            std::vector<unsigned short> vec;
            vec.reserve(1u << 16);
            for (int i = 0; i < (1u << 16); ++i) {
                vec.push_back(i);
            }
            std::random_shuffle(vec.begin(), vec.end());
            vec.resize(n);

            for (auto i : vec) {
                tree.insert(i);
            }
            auto iter = tree.begin();
            auto a = time_now();
            while (iter != tree.end()) iter++;
            auto b = time_now();
            return (b - a).count();
        }

        Result run() override {
            Result result;
            result.name = name;
            for (auto i = 1; i <= 3000; ++i) {
                auto t = run(i);
                if (i % 500 == 0) std::cout << name << ": " << i << std::endl;
                result.outcomes.emplace_back(i, t);
            }
            return result;
        }
    };

    IntSetIterationRunner<std::set<unsigned short>> stl_int_set_iteration("STLIntSetIteration");
    IntSetIterationRunner<VebTree<unsigned short>> veb_tree_iteration("VebTreeIteration");
    IntSetIterationRunner<BinaryTrie<unsigned short>> binary_trie_iteration("BinaryTrieIteration");
    IntSetIterationRunner<XFastTrie<unsigned short>> x_fast_trie_iteration("XFastTrieIteration");
    IntSetIterationRunner<YFastTrie<unsigned short>> y_fast_trie_iteration("YFastTrieIteration");
}
#endif //DATA_STRUCTURE_FOR_LOVE_INTSET_ITERATION_H
