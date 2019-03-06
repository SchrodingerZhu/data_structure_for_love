//
// Created by schrodinger on 3/4/19.
//
#include <binary_tree_base.hpp>
#include <iostream>
using namespace std;
using namespace data_structure;

int main(){
    BasicBinaryTree<int, std::string, size_t> tree {};
    tree.insert(1, "123");
    tree.insert(2, "234");
    tree.insert(-1, "234");
    tree.insert(125, "789");


    BasicBinaryTree<int, std::string, size_t> tree2 {};
    tree2.insert(4, "123");
    tree2.insert(5, "234");
    tree2.insert(6, "234");
    tree2.insert(7, "789");
    tree.absorb(tree2);
//    for (auto i : {500, 355, 32, 100, 0}) {
//        tree.insert(i);
//    }
    //tree.
    std::cout << tree.walk_to<OrderWalker>(120) << std::endl;
    struct ToVector: TreeVisitor<int, std::string, size_t > {
        std::vector<int> vec{};
        void visit(const int& key, std::string& value, size_t& info) override {
            vec.push_back(key);
        }
    };
    auto mm = ToVector{};
    tree.safe_in_order(mm);
    for(auto& i : mm.vec) {
        std::cout << i << std::endl;
    }
    auto m = tree.begin();
    for (auto i: tree) {
        std::cout << get<0>(i) << std::endl;
        get<1>(i) += '2';
    }
    for (auto i: tree) {
        std::cout << get<1>(i) << std::endl;
    }
    return 0;
}