//
// Created by schrodinger on 2/20/19.
//
#include <object_pool.hpp>
#include <cassert>
#include <vector>
#include <iostream>
#include <random>
#include <limits>
#include <chrono>
#include <memory>
#include <object_management.hpp>
std::mt19937_64 eng {};
std::uniform_int_distribution<int> dist(0, std::numeric_limits<int>::max());

#define get_rand() dist(eng)

inline auto random_string() {
    std::string test{};
    auto len = get_rand() % 233;
    while(len--) {
        test.push_back(static_cast<char>(get_rand() % 128));
    }
    return test;
}

int main() {
    using namespace std;
    using namespace data_structure;
    //auto mem_pool = data_structure::ObjectPool<int> {};
    std::vector<std::string, ObjectPool<string>> test_my;
    test_my.resize(100, "21355");
    for (auto &i : test_my) {
        assert(i == "21355");
    }
    std::vector<int> test_data {};
    std::vector<int *> test_pool_raw {};
    auto mem_pool = data_structure::ObjectPool<int> {};
    for(int i = 0; i < 5000; ++i) {
        auto num = get_rand();
        test_data.push_back(num);
        auto temp = mem_pool.construct_raw(num);
        test_pool_raw.push_back(temp);
    }
    std::clog << "testing basic alloc ";
    for (int i = 0; i < 5000; ++i) {
        assert(test_data[i] == *test_pool_raw[i]);
    }
    std::clog << "[Success]\n";
    std::clog << "testing basic recycle ";
    for(int i = 0; i < 5000; ++i) {
        mem_pool.recycle(test_pool_raw[i]);
        test_pool_raw.clear();
        test_data.clear();
    }
    for(int i = 0; i < 5500; ++i) {
        auto num = get_rand();
        test_data.push_back(num);
        auto temp = mem_pool.construct_raw(num);
        test_pool_raw.push_back(temp);
    }
    for (int i = 0; i < 5500; ++i) {
        assert(test_data[i] == *test_pool_raw[i]);
    }
    std::clog << "[Success]\n";

    auto str_pool = data_structure::ObjectPool<std::string> {};
    std::vector<std::string> test_strings {};
    std::vector<std::shared_ptr<string>> test_pool_complex;

    std::clog << "testing complex pool ";
    for(int i = 0; i < 5000; ++i) {
        auto str = random_string();
        test_strings.push_back(str);
        auto t = str_pool.construct_shared(str);
        test_pool_complex.emplace_back(std::move(t));
    }
    for (int i = 0; i < 5000; ++i) {
        assert(test_strings[i] == *test_pool_complex[i]);
    }
    std::clog << "[Success]\n";
    //quick_exit(0);
}
