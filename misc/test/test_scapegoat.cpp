//
// Created by schrodinger on 3/4/19.
//
#include <scapegoat.hpp>
#include <static_random_helper.hpp>
#include <set>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace data_structure;
using namespace data_structure::utils;
#define RANGE 100000
RandomIntGen<int> intGen{};

int main() {
    {
        ScapeGoat<int> test;
        set<int> test_set;
        for (int i = 0; i < RANGE; ++i) {
            auto m = intGen();
            test.insert(m);
            test_set.insert(m);
            assert(test.contains(m));
        }
        auto iter0 = test.begin();
        auto iter1 = test_set.begin();
        int time = 0;
        while (iter0 != test.end()) {
            time++;
            assert(*iter0 == *iter1);
            iter0++;
            iter1++;
        }
        assert(time == test.size());
        assert(time == test_set.size());
    }

    {
        ScapeGoat<int> test;
        set<int> test_set;
        for (int i = 0; i < RANGE; ++i) {
            auto m = intGen();
            test.insert(m);
            test_set.insert(m);
            assert(test.contains(m));
        }

        for (int i = 0; i < RANGE / 10; ++i) {
            auto m = intGen();
            test.erase(m);
            test_set.erase(m);
            assert(!test.contains(m));
        }
        auto iter0 = test.begin();
        auto iter1 = test_set.begin();
        int time = 0;
        while (iter0 != test.end()) {
            time++;
            assert(*iter0 == *iter1);
            ++iter0;
            iter1++;
        }
        assert(time == test.size());
        assert(time == test_set.size());
    }

}