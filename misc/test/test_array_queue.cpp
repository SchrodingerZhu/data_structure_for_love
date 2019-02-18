//
// Created by schrodinger on 2/18/19.
//
#define MEMORY_LEAK_TEST

#include <iostream>
#include <array_queue.hpp>

int main() {
    auto t = data_structure::ArrayQueue<int>();
    std::cout << t.size() << std::endl;
}

#undef MEMORY_LEAK_TEST
