//
// Created by schrodinger on 3/4/19.
//
#include <binary_tree_base.hpp>
#include <iostream>
using namespace std;
using namespace data_structure;

int main(){
    BasicBinaryTree<int, std::string> tree {};
    tree.insert(1, "123");
    tree.insert(2, "234");
    tree.insert(-1, "234");
    tree.insert(125, "789");
    for (auto i : {500, 355, 32, 100, 0}) {
        tree.insert(i);
    }
    //tree.
    std::cout << tree.walk_to<OrderWalker>(120) << std::endl;
    return 0;
}