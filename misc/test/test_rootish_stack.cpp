//
// Created by schrodinger on 2/18/19.
//
#define MEMORY_LEAK_TEST

#include <iostream>
#include <rootish_stack.hpp>
#include <vector>
#include <static_random_helper.hpp>

#define RANGE 100000
int main() {
    using namespace data_structure;
    utils::RandomIntGen<int> rand;
    RootishStack<int> test;
    std::vector<int> vec;

    for (auto i = 0; i < RANGE; ++i) {
        auto p = rand();
        vec.push_back(p);
        test.push_back(p);
    }

    auto iter = test.begin();
    for (auto i : vec) {
        assert(i == *iter);
        ++iter;
    }

    iter = test.begin();
    for (auto i : vec) {
        assert(i == *iter);
        iter++;
    }

    test.clear();
    vec.clear();

    for (auto i = 0; i < RANGE; ++i) {
        auto p = rand();
        vec.push_back(p);
        test.push_back(p);
    }

    auto j = 0;
    for (auto i : vec) {
        assert(i == test[j]);
        ++j;
    }

    for (int i = 0; i < RANGE / 100; ++i) {
        auto p = rand() % vec.size();
        vec.erase(vec.begin() + p);
        test.erase(p);
    }

    j = 0;
    for (auto i : vec) {
        assert(i == test[j]);
        ++j;
    }


}

#undef MEMORY_LEAK_TEST
