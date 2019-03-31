//
// Created by schrodinger on 3/18/19.
//
#define DEBUG
#define TEST_RANGE 100000
#include <algorithm>
#include <xor_list.hpp>
#include <list>
#include <random>
#include <chrono>

std::mt19937_64 eng{};
std::uniform_int_distribution<int> dist(0, std::numeric_limits<int>::max());
#define get_rand() dist(eng)

inline auto random_string() {
    std::string test{};
    auto len = get_rand() % 233;
    while (len--) {
        test.push_back(static_cast<char>(get_rand() % 128));
    }
    return test;
}
int main () {
    auto _time = std::chrono::steady_clock::now().time_since_epoch().count();
    eng.seed(static_cast<unsigned>(_time));
    using namespace data_structure;
    std::list<std::string> test0;
    XorList<std::string> test1;
    XorList<std::string, ObjectPool<XorNode<std::string>>> test2;
    for (int i = 0; i < TEST_RANGE; ++i) {
        auto p = random_string();
        test0.push_back(p);
        test1.push_back(p);
        test2.emplace_back(p);
    }
// Test Access
    {
        auto iter0 = test0.begin();
        auto iter1 = test1.begin();
        auto iter2 = test2.begin();
        for (int i = 0; i < TEST_RANGE; ++i) {
            assert(*iter0 == *iter1 && *iter0 == *iter2);
            ++iter0;
            ++iter1;
            iter2++;
        }
        assert(iter1 == test1.end());
        assert(iter2 == test2.end());
    }
// Test reverse visit
    {
        auto iter0 = test0.rbegin();
        auto iter1 = test1.rbegin();
        auto iter2 = test2.rbegin();
        for (int i = 0; i < TEST_RANGE; ++i) {
            assert(*iter0 == *iter1 && *iter0 == *iter2);
            ++iter0;
            ++iter1;
            iter2++;
        }
        assert(iter1 == test1.rend());
        assert(iter2 == test2.rend());
    }
// test contains
    {
        auto iter = test0.begin();
        for (int i = 0; i < 10; ++i) {
            assert(test1.contains(*iter));
            assert(test2.contains(*iter));
            iter++;
        }
    }
// fake sort
    {
        test0.sort();
        std::sort(test1.begin(), test1.end());
        std::sort(test2.begin(), test2.end());
        auto iter0 = test0.begin();
        auto iter1 = test1.begin();
        auto iter2 = test2.begin();
        for (int i = 0; i < TEST_RANGE; ++i) {
            assert(*iter0 == *iter1 && *iter0 == *iter2);
            ++iter0;
            ++iter1;
            iter2++;
        }
        assert(iter1 == test1.end());
        assert(iter2 == test2.end());
    }

    // fake sort
    {
        test0.sort();
        std::sort(test1.begin(), test1.end());
        std::sort(test2.begin(), test2.end());
        auto iter0 = test0.begin();
        auto iter1 = test1.begin();
        auto iter2 = test2.begin();
        for (int i = 0; i < TEST_RANGE; ++i) {
            assert(*iter0 == *iter1 && *iter0 == *iter2);
            ++iter0;
            ++iter1;
            iter2++;
        }
        assert(iter1 == test1.end());
        assert(iter2 == test2.end());
    }

    //test pop_back
    {
        for (int i = 0; i < TEST_RANGE / 10; ++i) {
            test1.pop_back();
            test2.pop_back();
        }
        auto iter1 = test1.begin();
        auto iter2 = test2.begin();
        for (int i = 0; i < test1.size(); ++i) {
            assert(*iter1 == *iter2);
            ++iter1;
            ++iter2;
        }
        assert(test1.size() == test2.size() && (test2.size() == TEST_RANGE - TEST_RANGE / 10));
        auto iter0 = test0.begin();
        auto n = TEST_RANGE * 9 / 10;
        while (n--) iter0++;
        for (; iter0 != test0.end(); ++iter0) {
            test1.push_back(*iter0);
            test2.push_back(*iter0);
        }
        assert(test1.size() == test2.size() && (test2.size() == TEST_RANGE));
        iter1 = test1.begin();
        iter2 = test2.begin();
        iter0 = test0.begin();
        for (int i = 0; i < test1.size(); ++i) {
            assert(*iter1 == *iter2 && *iter0 == *iter1);
            ++iter1;
            ++iter2;
            ++iter0;
        }
    }

    //test erase
//    {
//        for (int i = 0; i < 1000; ++i) {
//            auto p = get_rand() % test1.size();
//            test1.erase(p);
//            test2.erase(p);
//        }
//        auto iter1 = test1.begin();
//        auto iter2 = test2.begin();
//        for (int i = 0; i < TEST_RANGE; ++i) {
//            assert(*iter1 == *iter2);
//        }
//
//    }


}
#undef DEBUG