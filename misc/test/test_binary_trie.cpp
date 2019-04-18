//
// Created by schrodinger on 19-4-18.
//

#include <binary_trie.hpp>
#include <iostream>

int main() {
    using namespace data_structure;
    BinaryTrie<int> test;
    test.insert(15);
    test.insert(5);
    test.insert(99);
    test.insert(25);
    std::cout << test.contains(15) << std::endl;
    test.erase(15);
    test.erase(25);
    std::cout << test.contains(9999) << std::endl;
}