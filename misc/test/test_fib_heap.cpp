//
// Created by schrodinger on 19-4-9.
//
#include <fib_heap.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <cassert>

std::mt19937_64 eng{};
std::uniform_int_distribution<int> dist(0, std::numeric_limits<int>::max());
#define RANGE 100000
#define get_rand() dist(eng)

inline auto random_string() {
    std::string test{};
    auto len = get_rand() % 233;
    while (len--) {
        test.push_back(static_cast<char>(get_rand() % 128));
    }
    return test;
}

signed main() {
    using namespace data_structure;
    FibHeap<std::string> test;
    FibHeap<std::string, std::greater<>> test2;
    std::vector<std::string> temp, temp2, res, res2;
    for (auto i = 0; i < RANGE; ++i) {
        auto m = random_string();
        test.push(m);
        temp.push_back(m);
        m = random_string();
        temp2.push_back(m);
        test2.emplace(std::move(m));
    }
    while (test.size()) {
        res.push_back(test.top());
        test.pop();
    }
    while (test2.size()) {
        res2.push_back(test2.top());
        test2.pop();
    }
    std::sort(temp.begin(), temp.end());
    std::sort(temp2.begin(), temp2.end(), std::greater<std::string>{});
    assert(temp == res);
    assert(temp2 == res2);
    std::vector<int> a = {9, 6, 5, 2, 1, 0, -1, 2, 3}, b;
    std::sort(a.begin(), a.end());

    FibHeap<int> to_move = {9, 6, 5, 2, 1, 0, -1, 2, 3}, move_res;
    move_res = std::move(to_move);
    std::vector<int> d;
    while (move_res.size()) {
        d.push_back(move_res.top());
        move_res.pop();
    }
    assert(a == d);

    FibHeap<int> decrease1 = {1, 3, 5, 1, 2}, decrease2 = {1, 3, 5, 1, 2};
    auto ha = decrease1.emplace_and_hold(9);
    ha.re_emplace(-1);
    std::vector<int> res_ha = {-1, 1, 1, 2, 3, 5}, test_ha;
    while (decrease1.size()) {
        test_ha.push_back(decrease1.top());
        decrease1.pop();
    }
    assert(res_ha == test_ha);
    auto hb = decrease2.push_and_hold(9);
    hb.re_push(4);
    hb.re_emplace(-2);
    std::vector<int> res_hb = {-2, 1, 1, 2, 3, 5}, test_hb;
    while (decrease2.size()) {
        test_hb.push_back(decrease2.top());
        decrease2.pop();
    }
    assert(res_hb == test_hb);

    FibHeap<int> ka = {5, 7, 6, 3, 2};
    FibHeap<int> kb = {4, 7, -1, 9};
    std::vector<int> kv = {5, 7, 6, 3, 2, 4, 7, -1, 9}, rv;
    ka.merge(kb);
    while (ka.size()) {
        rv.push_back(ka.top());
        ka.pop();
    }
    std::sort(kv.begin(), kv.end());
    assert(kv == rv);
    return 0;
}