//
// Created by schrodinger on 3/18/19.
//
#define DEBUG
#include <algorithm>
#include <xor_list.hpp>
#include <iostream>

int main () {
    using namespace data_structure;
    XorList<std::string> test;

    std::string n = "234";

    test.emplace_back(std::move(n));
    test.emplace_back("123");
    test.push_back("012");

    auto i = test.crend();
    for(--i; i != test.crbegin(); --i) {
        std::cout << *i << std::endl;
    }

    std::cout << *i << std::endl;
    std::sort(test.begin(), test.end());
    for (const auto& j : test) {
        std::cout << j << std::endl;
    }
    test.pop_back();
    //test.view();
}
#undef DEBUG