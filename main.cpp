#include <iostream>
#include <string>
#include "optimized_vector.hpp"
#include <chrono>
#include <vector>
int main() {
    data_structure::optimized_vector<int, 2> test;
    for(int i = 0; i < 10; ++i) {
        test.push_back(i);
    }
    for(const auto& i : test) {
        std::cout << i << ", ";
    }

    std::cout << std::endl;
    return 0;
}