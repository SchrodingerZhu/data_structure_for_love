//
// Created by schrodinger on 3/4/19.
//
#include <binary_tree_base.hpp>
#include <iostream>
using namespace std;
using namespace data_structure;

int main(){
    BinaryTreeBase<int, std::string> tree {};
    tree.insert(1, "123");
    tree.insert(2, "234");
    tree.insert(-1, "234");
    tree.insert(125, "789");

    std::cout << tree.walk_to<OrderWalker<size_t>>(120) << std::endl;
    return 0;
}