//
// Created by schrodinger on 19-4-18.
//

#include <binary_trie.hpp>
#include <iostream>

int main() {
    using namespace data_structure;
    BinaryTrie<int, 4> test;
    test.insert(15);
    std::cout << test.contains(15) << std::endl;
}