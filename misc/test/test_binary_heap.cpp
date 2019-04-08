#include <iostream>
#include <binary_heap.hpp>
#include <cstdlib>
#include <object_pool.hpp>
int main() {
    using namespace data_structure;
    BinaryHeap<std::string, std::less<>> bheap {};
    auto n = 10;
    while(n--) {
        bheap.emplace(" e21341425 1");
    }
    bheap.debug();
    return 0;
}