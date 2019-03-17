//
// Created by schrodinger on 3/18/19.
//

#include <xor_list.hpp>
#include <iostream>
int main () {
    using namespace data_structure;
    XorList<std::string> test;
    test.insert("123");
    test.view();
}