//
// Created by schrodinger on 3/18/19.
//
#define DEBUG
#include <algorithm>
#include <xor_list.hpp>
#include <iostream>
#include <list>
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

    for (const auto& j : test) {
        std::cout << j << std::endl;
    }
    test.pop_back();
    //XorList<long long> test2;
    std::list<long long> test2;
    for(long long p = 0; p <= 50000; ++p) {
        test2.push_back(rand());
    }
    //test2.view();
    std::cout << std::endl;
    test2.sort();
    //std::sort(test2.begin(), test2.end());
    //std::__sort(test.begin(), test.end(), std::less<long long>());
    //test2.view();
    //test2.view();
}
#undef DEBUG