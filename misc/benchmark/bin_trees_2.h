//
// Created by Administrator on 2019/5/17.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_2
#define DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_2

#include "benchmark.h"
#include <avl_tree.hpp>
#include <rb_tree.hpp>
#include <splay.hpp>
#include <treap.hpp>
#include <scapegoat.hpp>
#include <set>

namespace benchmark {
    using namespace data_structure;

    struct TreeRunner : public BenchMark {
        std::string name;

        explicit TreeRunner(std::string name) noexcept : name(std::move(name)), BenchMark() {}
    };


    template<class Tree>
    struct RandomInsertionRunner : public TreeRunner {


        explicit RandomInsertionRunner(std::string name) noexcept : TreeRunner(std::move(name)) {}

        long long run(size_t n) {
            Tree tree;
            std::vector<int> vec;
            gen_random_int(vec, n);
            auto a = time_now();
            for (auto i : vec) {
                tree.insert(i);
            }
            auto b = time_now();
            return (b - a).count();
        }

        Result run() final {
            Result result;
            result.name = name;
            for (auto i = 1; i <= 10000; ++i) {
                auto t = run(i);
                result.outcomes.emplace_back(i, t);
            }
            return result;
        }
    };

    RandomInsertionRunner<AVLTree<int>> avl_random_insertion("AVLTreeRandomInsertion");
    RandomInsertionRunner<RbTree<int>> rb_random_insertion("RbTreeRandomInsertion");
    RandomInsertionRunner<Splay<int>> splay_random_insertion("SplayTreeRandomInsertion");
    RandomInsertionRunner<Treap<int>> treap_random_insertion("TreapTreeRandomInsertion");
    RandomInsertionRunner<ScapeGoat<int>> scapegoat_random_insertion("ScapegoatTreeRandomInsertion");
    RandomInsertionRunner<std::set<int>> set_random_insertion("SetTreeRandomInsertion");
}
#endif //DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_2
