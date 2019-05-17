//
// Created by Administrator on 2019/5/17.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_4
#define DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_4

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
    struct CheckingRunner : public TreeRunner {


        explicit CheckingRunner(std::string name) noexcept : TreeRunner(std::move(name)) {}

        long long run(size_t n) {
            Tree tree;
            std::vector<int> vec;
            gen_random_int(vec, n);

            for (auto i : vec) {
                tree.insert(i);
            }
            auto a = time_now();
            for (auto i = 0; i < vec.size(); ++i) {
                tree.contains(rand());
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

    template<>
    struct CheckingRunner<std::set<int>> : public TreeRunner {
        explicit CheckingRunner(std::string name) noexcept : TreeRunner(std::move(name)) {}

        long long run(size_t n) {
            std::set<int> tree;
            std::vector<int> vec;
            gen_random_int(vec, n);

            for (auto i : vec) {
                tree.insert(i);
            }
            auto a = time_now();
            for (auto i = 0; i < vec.size(); ++i) {
                tree.count(rand());
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

    CheckingRunner<AVLTree<int>> avl_checking("AVLTreeChecking");
    CheckingRunner<RbTree<int>> rb_checking("RbTreeChecking");
    CheckingRunner<Splay<int>> splay_checking("SplayTreeChecking");
    CheckingRunner<Treap<int>> treap_checking("TreapTreeChecking");
    CheckingRunner<ScapeGoat<int>> scapegoat_checking("ScapegoatTreeChecking");
    CheckingRunner<std::set<int>> set_checking("SetTreeChecking");


    template<class Tree>
    struct OrderedCheckingRunner : public TreeRunner {


        explicit OrderedCheckingRunner(std::string name) noexcept : TreeRunner(std::move(name)) {}

        long long run(size_t n) {
            Tree tree;
            std::vector<int> vec;
            gen_random_int(vec, n);

            for (auto i : vec) {
                tree.insert(i);
            }
            auto a = time_now();
            for (auto i = 0; i < 100'000; ++i) {
                tree.contains(i);
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

    template<>
    struct OrderedCheckingRunner<std::set<int>> : public TreeRunner {
        explicit OrderedCheckingRunner(std::string name) noexcept : TreeRunner(std::move(name)) {}

        long long run(size_t n) {
            std::set<int> tree;
            std::vector<int> vec;
            gen_random_int(vec, n);

            for (auto i : vec) {
                tree.insert(i);
            }
            auto a = time_now();
            for (auto i = 0; i < 100'000; ++i) {
                tree.count(i);
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

    OrderedCheckingRunner<AVLTree<int>> avl_ordered_checking("AVLTreeOrderedChecking");
    OrderedCheckingRunner<RbTree<int>> rb_ordered_checking("RbTreeOrderedChecking");
    OrderedCheckingRunner<Splay<int>> splay_ordered_checking("SplayTreeOrderedChecking");
    OrderedCheckingRunner<Treap<int>> treap_ordered_checking("TreapTreeOrderedChecking");
    OrderedCheckingRunner<ScapeGoat<int>> scapegoat_ordered_checking("ScapegoatTreeOrderedChecking");
    OrderedCheckingRunner<std::set<int>> set_ordered_checking("SetTreeOrderedChecking");

}
#endif //DATA_STRUCTURE_FOR_LOVE_BIN_TREES_H_4
