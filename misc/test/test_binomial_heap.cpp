//
// Created by schrodinger on 19-4-9.
//
#include <binomial_heap.hpp>
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
    BinomialHeap<std::string> test;
    BinomialHeap<std::string, std::greater<>> test2;
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
    BinomialHeap<int> to_copy = {9, 6, 5, 2, 1, 0, -1, 2, 3}, copy_res;
    copy_res = to_copy;
    std::vector<int> a = {9, 6, 5, 2, 1, 0, -1, 2, 3}, b;
    while (copy_res.size()) {
        b.push_back(copy_res.top());
        copy_res.pop();
    }
    std::sort(a.begin(), a.end());
    assert(a == b);

    BinomialHeap<int> to_move = {9, 6, 5, 2, 1, 0, -1, 2, 3}, move_res;
    copy_res = std::move(to_move);
    std::vector<int> d;
    while (copy_res.size()) {
        d.push_back(copy_res.top());
        copy_res.pop();
    }
    assert(a == d);
}