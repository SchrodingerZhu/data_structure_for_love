//
// Created by Administrator on 2019/5/17.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_3
#define DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_3

#include "benchmark.h"
#include <avl_tree.hpp>
#include <rb_tree.hpp>
#include <splay.hpp>
#include <treap.hpp>
#include <scapegoat.hpp>
#include <algorithm>
#include <set>

namespace benchmark {
    using namespace data_structure;

#ifndef DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_1

    struct TreeRunner : public BenchMark {
        std::string name;

        explicit TreeRunner(std::string name) noexcept : name(std::move(name)), BenchMark() {}
    };

#endif


    template<class Tree>
    struct RandomDeletionRunner : public TreeRunner {


        explicit RandomDeletionRunner(std::string name) noexcept : TreeRunner(std::move(name)) {}

        long long run(size_t n) {
            Tree tree;
            std::vector<int> vec;
            gen_random_int(vec, n);

            for (auto i : vec) {
                tree.insert(i);
            }
            std::random_shuffle(vec.begin(), vec.end());
            auto a = time_now();
            for (auto i : vec) {
                tree.erase(i);
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

    RandomDeletionRunner<AVLTree<int>> avl_random_deletion("AVLTreeRandomDeletion");
    RandomDeletionRunner<RbTree<int>> rb_random_deletion("RbTreeRandomDeletion");
    RandomDeletionRunner<Splay<int>> splay_random_deletion("SplayTreeRandomDeletion");
    RandomDeletionRunner<Treap<int>> treap_random_deletion("TreapTreeRandomDeletion");
    RandomDeletionRunner<ScapeGoat<int>> scapegoat_random_deletion("ScapegoatTreeRandomDeletion");
    RandomDeletionRunner<std::set<int>> set_random_deletion("SetTreeRandomDeletion");

    template<class Tree>
    struct IterationRunner : public TreeRunner {


        explicit IterationRunner(std::string name) noexcept : TreeRunner(std::move(name)) {}

        long long run(size_t n) {
            Tree tree;
            std::vector<int> vec;
            gen_random_int(vec, n);

            for (auto i : vec) {
                tree.insert(i);
            }
            std::random_shuffle(vec.begin(), vec.end());
            auto a = time_now();
            auto m = tree.begin();
            while (m != tree.end()) {
                ++m;
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

    IterationRunner<AVLTree<int>> avl_iteration("AVLTreeIteration");
    IterationRunner<RbTree<int>> rb_iteration("RbTreeIteration");
    IterationRunner<Splay<int>> splay_iteration("SplayTreeIteration");
    IterationRunner<Treap<int>> treap_iteration("TreapTreeIteration");
    IterationRunner<ScapeGoat<int>> scapegoat_iteration("ScapegoatTreeIteration");
    IterationRunner<std::set<int>> set_iteration("SetTreeIteration");
}
#endif //DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_3
