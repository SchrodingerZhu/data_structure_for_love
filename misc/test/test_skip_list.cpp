//
// Created by schrodinger on 19-5-20.
//

#include <skip_list.hpp>
#include <iostream>
#include <vector>
#include <static_random_helper.hpp>
#include <cassert>
#include <van_emde_boas.hpp>

#define TEST_RANGE 50000
using namespace data_structure;

int main() {
    {
        SkipList<int> test;
        utils::RandomIntGen<int> rand;
        std::vector<int> vec;
        for (int i = 0; i < TEST_RANGE; ++i) {
            auto m = rand();
            test.insert(m);
            vec.push_back(m);
        }
        std::sort(vec.begin(), vec.end());
        auto p = test.begin();
        for (int i : vec) {
            assert(test.contains(i));
            assert(i == *p);
            p++;
        }
    }
    {
        SkipList<int> test;
        utils::RandomIntGen<int> rand;
        std::vector<int> vec;
        for (int i = 0; i < TEST_RANGE; ++i) {
            auto m = rand();
            test.insert(m);
            vec.push_back(m);
        }
        std::random_shuffle(vec.begin(), vec.end());
        for (int i = 0; i < TEST_RANGE / 10; ++i) {
            test.erase(vec.back());
            vec.pop_back();
        }
        std::sort(vec.begin(), vec.end());
        auto p = test.begin();
        for (int i : vec) {
            assert(test.contains(i));
            assert(i == *p);
            p++;
        }
    }

    {
        SkipList<int> test;
        utils::RandomIntGen<int> rand;
        VebTree<int> tree;
        for (int i = 0; i < TEST_RANGE; ++i) {
            auto m = rand();
            test.insert(m);
            tree.insert(m);
        }

        for (int i = 0; i < TEST_RANGE; ++i) {
            auto m = rand();
            assert(test.succ(m) == tree.succ(m));
            assert(test.pred(m) == tree.pred(m));
        }


    }
}