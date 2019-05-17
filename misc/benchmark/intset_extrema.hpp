//
// Created by schrodinger on 19-5-18.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_INTSET_EXTREMA_HPP
#define DATA_STRUCTURE_FOR_LOVE_INTSET_EXTREMA_HPP

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
    struct IntSetExtremaRunner : public IntsetRunner {


        explicit IntSetExtremaRunner(std::string name) noexcept : IntsetRunner(std::move(name)) {}

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
            auto a = time_now();
            tree.min();
            tree.max();
            auto b = time_now();
            return (b - a).count();
        }

        Result run() override {
            Result result;
            result.name = name;
            for (auto i = 1; i <= 5000; ++i) {
                auto t = run(i);
                if (i % 500 == 0) std::cout << name << ": " << i << std::endl;
                result.outcomes.emplace_back(i, t);
            }
            return result;
        }
    };

    template<>
    struct IntSetExtremaRunner<std::set<unsigned short>> : public IntsetRunner {


        explicit IntSetExtremaRunner(std::string name) noexcept : IntsetRunner(std::move(name)) {}

        long long run(size_t n) {
            std::set<unsigned short> tree;
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
            auto a = time_now();
            tree.begin();
            --tree.end();
            auto b = time_now();
            return (b - a).count();
        }

        Result run() override {
            Result result;
            result.name = name;
            for (auto i = 1; i <= 5000; ++i) {
                auto t = run(i);
                if (i % 500 == 0) std::cout << name << ": " << i << std::endl;
                result.outcomes.emplace_back(i, t);
            }
            return result;
        }
    };

    IntSetExtremaRunner<std::set<unsigned short>> stl_int_set_extrema("STLIntSetExtrema");
    IntSetExtremaRunner<VebTree<unsigned short>> veb_tree_extrema("VebTreeExtrema");
    IntSetExtremaRunner<BinaryTrie<unsigned short>> binary_trie_extrema("BinaryTrieExtrema");
    IntSetExtremaRunner<XFastTrie<unsigned short>> x_fast_trie_extrema("XFastTrieExtrema");
    IntSetExtremaRunner<YFastTrie<unsigned short>> y_fast_trie_extrema("YFastTrieExtrema");
}
#endif //DATA_STRUCTURE_FOR_LOVE_INTSET_EXTREMA_HPP
