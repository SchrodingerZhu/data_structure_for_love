//
// Created by schrodinger on 4/2/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BINARY_HEAP_HPP
#define DATA_STRUCTURE_FOR_LOVE_BINARY_HEAP_HPP

#include <heap_base.hpp>
namespace data_structure {
    template <typename T, typename Compare = std::less<T>, template <class> typename Alloc = std::allocator>
            class BinaryHeap: public Heap<T, Compare> {
                Alloc<T> alloc {};
            };
}

#endif //DATA_STRUCTURE_FOR_LOVE_BINARY_HEAP_HPP
