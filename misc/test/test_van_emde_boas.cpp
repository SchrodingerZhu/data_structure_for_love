//
// Created by schrodinger on 19-4-11.
//

#include <van_emde_boas.hpp>
#include <iostream>
#include <random>
#include <set>
#include <cassert>
#include "play_test.cpp"
std::mt19937_64 eng{};
std::uniform_int_distribution<short> dist(0, std::numeric_limits<short>::max());
#define RANGE 1000000
#define get_rand() dist(eng)

int main() {
    using namespace data_structure;
    VebTree<short> test;
    std::set<short> test_set;
    for (int i = 0; i < RANGE; ++i) {
        auto t = get_rand() % 65536;
        //std::cout << t << std::endl;
        test_set.insert(t);
        test.insert(t);
        //test.debug();
        //std::cout << "----------------------------------------------------------" << std::endl;
    }
    std::cout << std::endl;

    auto set_iter = test_set.begin();
    auto veb_iter = test.begin();
    for (int i = 0; i < test_set.size(); ++i) {
        //std::cout << *set_iter << " " << *veb_iter << std::endl;
        assert(*set_iter == *veb_iter);
        set_iter++;
        ++veb_iter;
    }
}