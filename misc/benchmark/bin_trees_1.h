//
// Created by Administrator on 2019/5/17.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_1
#define DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_1

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
    struct OrderedInsertionRunner : public TreeRunner {


        explicit OrderedInsertionRunner(std::string name) noexcept : TreeRunner(std::move(name)) {}

        long long run(size_t n) {
            Tree tree;
            std::vector<int> order_int_vec;
            gen_ordered_int(order_int_vec, n);
            auto a = time_now();
            for (auto i : order_int_vec) {
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

    OrderedInsertionRunner<AVLTree<int>> avl_ordered_insertion("AVLTreeOrderedInsertion");
    OrderedInsertionRunner<RbTree<int>> rb_ordered_insertion("RbTreeOrderedInsertion");
    OrderedInsertionRunner<Splay<int>> splay_ordered_insertion("SplayTreeOrderedInsertion");
    OrderedInsertionRunner<Treap<int>> treap_ordered_insertion("TreapTreeOrderedInsertion");
    OrderedInsertionRunner<ScapeGoat<int>> scapegoat_ordered_insertion("ScapegoatTreeOrderedInsertion");
    OrderedInsertionRunner<std::set<int>> set_ordered_insertion("SetTreeOrderedInsertion");
}
#endif //DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_1
