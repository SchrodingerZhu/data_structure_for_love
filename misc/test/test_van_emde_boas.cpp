//
// Created by schrodinger on 19-4-11.
//
#define DEBUG
#include <van_emde_boas.hpp>
#include <iostream>
#include <random>
#include <set>
#include <cassert>
#include <chrono>
std::mt19937_64 eng{};
std::uniform_int_distribution<short> dist(0, std::numeric_limits<short>::max());
#define RANGE 100000
#define get_rand() dist(eng)

int main() {
    eng.seed(std::chrono::system_clock::now().time_since_epoch().count());
    using namespace data_structure;
    {
        VebTree<int> test;
        std::set<int> test_set;
        for (int i = 0; i < RANGE; ++i) {
            auto t = get_rand() % RANGE;
            if (!test_set.count(t)) {
                test_set.insert(t);
                test.insert(t);
                assert(test.contains(t));
            }
        }

        //return 0;
        auto set_iter = test_set.begin();
        auto veb_iter = test.begin();
        for (int i = 0; i < test_set.size(); ++i) {
            assert(*set_iter == *veb_iter);
            set_iter++;
            ++veb_iter;
        }
    }
    {
        VebTree<int> test;
        std::set<int> test_set;
        for (int i = 0; i < RANGE; ++i) {
            auto t = get_rand() % RANGE;
            if (!test_set.count(t)) {
                test_set.insert(t);
                test.insert(t);
                assert(test.contains(t));
            }
            assert(*test_set.begin() == test.min());
            assert(*test_set.rbegin() == test.max());
        }
        for (int i = 0; i < RANGE; ++i) {
            auto t = get_rand() % RANGE;
            if (test_set.count(t)) {
                assert(test.contains(t));
                test_set.erase(t);
                test.erase(t);
                assert(!test.contains(t));
                if (test_set.empty()) break;
                //std::cout << *test_set.begin() << " " << test.min().value() << std::endl;
                assert(*test_set.begin() == test.min());
                assert(*test_set.rbegin() == test.max());
            }
        }
        auto set_iter = test_set.begin();
        auto veb_iter = test.begin();
        for (int i = 0; i < test_set.size(); ++i) {
            assert(*set_iter == *veb_iter);
            set_iter++;
            ++veb_iter;
        }
    }

    {
        VebTree<int> p;
        std::set<int> test_set;
        for (int i = 0; i < RANGE; ++i) {
            auto t = get_rand() % RANGE;
            if (!test_set.count(t)) {
                test_set.insert(t);
                p.insert(t);
                assert(p.contains(t));
            }
            assert(*test_set.begin() == p.min());
            assert(*test_set.rbegin() == p.max());
        }
        auto test = p;
        auto set_iter = test_set.begin();
        auto veb_iter = test.begin();
        for (int i = 0; i < test_set.size(); ++i) {
            assert(*set_iter == *veb_iter);
            set_iter++;
            ++veb_iter;
        }
        for (int i = 0; i < RANGE; ++i) {
            auto t = get_rand() % RANGE;
            if (test_set.count(t)) {
                assert(test.contains(t));
                test_set.erase(t);
                test.erase(t);
                assert(!test.contains(t));
                if (test_set.empty()) break;
                //std::cout << *test_set.begin() << " " << test.min().value() << std::endl;
                assert(*test_set.begin() == test.min());
                assert(*test_set.rbegin() == test.max());
            }
        }
        set_iter = test_set.begin();
        veb_iter = test.begin();
        for (int i = 0; i < test_set.size(); ++i) {
            assert(*set_iter == *veb_iter);
            set_iter++;
            ++veb_iter;
        }
    }

    {
        VebTree<int> p = {1, 3, 5, 7, 9, 12, 5261, 991208};
        std::set<int> test_set = {1, 3, 5, 7, 9, 12, 5261, 991208};
        for (int i = 0; i < RANGE; ++i) {
            auto t = get_rand() % RANGE;
            if (!test_set.count(t)) {
                test_set.insert(t);
                p.insert(t);
                assert(p.contains(t));
            }
            assert(*test_set.begin() == p.min());
            assert(*test_set.rbegin() == p.max());
        }
        auto test = std::move(p);
        auto set_iter = test_set.begin();
        auto veb_iter = test.cbegin();
        for (; veb_iter != test.cend(); veb_iter++) {
            assert(*set_iter == *veb_iter);
            set_iter++;
        }
        for (int i = 0; i < RANGE; ++i) {
            auto t = get_rand() % RANGE;
            if (test_set.count(t)) {
                assert(test.contains(t));
                test_set.erase(t);
                test.erase(t);
                assert(!test.contains(t));
                if (test_set.empty()) break;
                //std::cout << *test_set.begin() << " " << test.min().value() << std::endl;
                assert(*test_set.begin() == test.min());
                assert(*test_set.rbegin() == test.max());
            }
        }
        set_iter = test_set.begin();
        veb_iter = test.cbegin();
        for (int i = 0; i < test_set.size(); ++i) {
            assert(*set_iter == *veb_iter);
            set_iter++;
            ++veb_iter;
        }
    }


}