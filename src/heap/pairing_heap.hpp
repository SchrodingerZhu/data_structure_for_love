//
// Created by schrodinger on 19-4-9.
//
#include <heap_base.hpp>

#ifndef DATA_STRUCTURE_FOR_LOVE_PAIRING_HEAP_HPP
#define DATA_STRUCTURE_FOR_LOVE_PAIRING_HEAP_HPP
namespace data_structure {
    template<typename T>
    struct ParingHeapNode {
    };

    template<typename T, typename Compare>
    class PairingHeap : Heap<T, Compare> {
    };
}
#endif //DATA_STRUCTURE_FOR_LOVE_PAIRING_HEAP_HPP
