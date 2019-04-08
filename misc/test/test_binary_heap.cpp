#include <iostream>
#include <binary_heap.hpp>
#include <cstdlib>
#include <object_pool.hpp>
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
int main() {
    using namespace data_structure;
    BinaryHeap<int, std::less<>, ObjectPool<BinaryHeapNode<int>>> bHeap_one {};
    BinaryHeap<std::string> bHeap_two {};
    auto n = 114514;
    std::vector<int> test_one, res_one;
    while(n--) {
        test_one.emplace_back(get_rand());
        bHeap_one.emplace(test_one.back());
    }
    while(bHeap_one.size()) {
        res_one.emplace_back(bHeap_one.top());
        bHeap_one.pop();
    }
    std::sort(test_one.begin(), test_one.end());
    assert(test_one == res_one);
    n = 5261;
    std::vector<std::string> test_two, res_two;
    while(n--) {
        test_two.emplace_back(random_string());
        bHeap_two.push(test_two.back());
    }
    while(bHeap_two.size()) {
        res_two.emplace_back(bHeap_two.top());
        bHeap_two.pop();
    }
    std::sort(test_two.begin(), test_two.end());
    assert(test_two == res_two);
    return 0;
}