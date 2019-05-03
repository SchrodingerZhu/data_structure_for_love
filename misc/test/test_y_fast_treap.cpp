//
// Created by schrodinger on 19-4-30.
//
#define DEBUG

#include <set>
#include <yfast_treap.hpp>
#include <iostream>
#include <static_random_helper.hpp>
#include <cassert>

int main() {

    using namespace data_structure::utils;
    RandomIntGen<long> rand;
    YTreap<long> test;
    for (int i = 0; i < 10000; ++i) {
        auto k = rand();
        test.insert(k);
        assert(test.contains(k));
    }

    for (int i = 0; i < 100000; ++i) {
        auto k = rand();
        test.erase(k);
        assert(!test.contains(k));
    }


    for (int i = 0; i < 100000; ++i) {
        test.insert(i);
        assert(test.contains(i));
    }

    for (int i = 0; i < 100000; ++i) {
        assert(test.contains(i));
    }

    std::set<long> base;
    YTreap<long> a, b;
    for (int i = 0; i < 10000; ++i) {
        auto k = rand() % 100000;
        a.insert(k);
        base.insert(k);
        assert(a.contains(k));
    }

    for (int i = 0; i < 10000; ++i) {
        auto k = rand() % 100000 + 100000;
        b.insert(k);
        base.insert(k);
        assert(b.contains(k));
    }

    b.absorb_smaller(a);
    for (auto i : base) {
        assert(b.contains(i));
        b.erase(i);
        assert(!b.contains(i));
    }

    auto p = std::move(b);
    {
        YTreap<long> s;
        for (auto i = 0; i <= 5000; ++i) {
            s.insert(i);
        }
        auto t = s.split(2550);
        for (auto i = 0; i <= 2550; ++i) {
            assert(!s.contains(i));
            assert(t.contains(i));
        }
        for (auto i = 2551; i <= 5000; ++i) {
            assert(s.contains(i));
            assert(!t.contains(i));
        }
    }


    return 0;
}

#undef DEBUG