//
// Created by schrodinger on 4/2/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_HEAP_BASE_HPP
#define DATA_STRUCTURE_FOR_LOVE_HEAP_BASE_HPP

#include <functional>
namespace data_structure {
    template<typename T, typename Compare = std::less<T>>
    class Heap {

        virtual const T &top() = 0;
        virtual void push(const T& t) = 0;
        virtual size_t size() = 0;
        virtual void pop() = 0;
    };
}
#endif //DATA_STRUCTURE_FOR_LOVE_HEAP_BASE_HPP
