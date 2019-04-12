//
// Created by schrodinger on 19-4-11.
//

#include <van_emde_boas.hpp>
#include <iostream>
#include <random>
#include <set>
#include <cassert>

std::mt19937_64 eng{};
std::uniform_int_distribution<int> dist(0, std::numeric_limits<int>::max());
#define RANGE 10
#define get_rand() dist(eng)

inline auto random_string() {
    std::string test{};
    auto len = get_rand() % 233;
    while (len--) {
        test.push_back(static_cast<char>(get_rand() % 128));
    }
    return test;
}
int main() {
    using namespace data_structure;
    VebTree<int> test;
    std::set<int> test_set;
    for (int i = 0; i < RANGE; ++i) {
        auto t = get_rand();
        std::cout << t << std::endl;
        if (!test_set.count(t)) {
            test_set.insert(t);
            test.insert(t);
            assert(test.contains(t));
        }
    }
    auto set_iter = test_set.begin();
    auto veb_iter = test.begin();
    for (int i = 0; i < test_set.size(); ++i) {
        std::cout << *set_iter << " " << *veb_iter << std::endl;
        //assert(*set_iter == *veb_iter);
        set_iter++;
        ++veb_iter;
    }
}