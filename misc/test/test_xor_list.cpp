//
// Created by schrodinger on 3/18/19.
//
// 安得万能杀虫剂
// 灭遍BUG心不累
// 归并排序未成型
// 求助天神DEBUG
/// @attention 组员们，看你们的了！
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
    XorList<long long> test2;
    for(long long p = 0; p <= 4; ++p) {
        test2.push_back(rand());
    }
    for (const auto& j : test2) {
        std::cout << j << std::endl;
    }
    std::cout << std::endl;
    //test2.sort();

    //std::sort(test2.begin(), test2.end());
    //std::__sort(test.begin(), test.end(), std::less<long long>());
    for (const auto& j : test2) {
        std::cout << j << std::endl;
    }
    //test2.view();
}
#undef DEBUG