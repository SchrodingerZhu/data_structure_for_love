//
// Created by schrodinger on 19-4-30.
//
#include <yfast_treap.hpp>
#include <static_random_helper.hpp>

#define DEBUG

#include <cassert>

int main() {

    using namespace data_structure::utils;
    RandomRealGen<double> rand;
    YTreap<double> test;
    for (int i = 0; i < 1000; ++i) {
        auto k = rand();
        test.insert(k);
        assert(test.contains(k));
    }

    for (int i = 0; i < 100000; ++i) {
        auto k = rand();
        test.remove(k);
        assert(!test.contains(k));
    }

    for (int i = 0; i < 100000; ++i) {
        test.insert(i);
        assert(test.contains(i));
    }

    return 0;
}