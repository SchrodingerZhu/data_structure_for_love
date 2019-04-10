//
// Created by schrodinger on 19-4-10.
//

#include <fib_heap.hpp>
#include <iostream>

int main() {
    using namespace data_structure;
    FibHeap<int> test, test2;
    test.push(1);
    auto m = test.push_and_hold(2);
    test.push(0);
    test2.push(-1);
    test2.push(-9);
    test.merge(test2);
    m.re_emplace(-10);
    m.re_emplace(-11);
    while (test.size()) {
        std::cout << test.top() << std::endl;
        test.pop();
    }
}