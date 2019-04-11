//
// Created by schrodinger on 19-4-11.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_INTEGER_SET_BASE_HPP
#define DATA_STRUCTURE_FOR_LOVE_INTEGER_SET_BASE_HPP

#include <utility>
#include <cstddef>

namespace data_structure {
    template<typename Int, typename = std::enable_if_t<std::is_integral_v<Int>>>
    class IntegerSetBase {
    public:
        virtual bool contains(Int t) const = 0;

        virtual Int pred(Int t) const = 0;

        virtual Int succ(Int t) const = 0;

        virtual Int max() const = 0;

        virtual Int min() const = 0;

        virtual void insert(Int t) = 0;

        virtual void erase(Int t) = 0;
    };
}
#endif //DATA_STRUCTURE_FOR_LOVE_INTEGER_SET_BASE_HPP
