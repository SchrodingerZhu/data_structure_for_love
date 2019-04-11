//
// Created by schrodinger on 19-4-11.
//

#include <van_emde_boas.hpp>
#include <iostream>

int main() {
    using namespace data_structure;
    VebTree<int> test;
    test.insert(9);
    test.insert(7000);
    test.insert(7001);
    std::cout << test.contains(7001) << " " << std::endl;
    auto m = test.succ(7001);

    if (m) {
        std::cout << m.value() << std::endl;
    } else {
        std::cout << "Nothing" << std::endl;
    }
    for (auto i: test) {
        std::cout << i << std::endl;
    }

    auto test2 = std::move(test);
    for (auto i: test2) {
        std::cout << i << std::endl;
    }
    return 0;
}