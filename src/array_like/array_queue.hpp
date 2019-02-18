//
// Created by schrodinger on 2/18/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_ARRAY_QUEUE_HPP
#define DATA_STRUCTURE_FOR_LOVE_ARRAY_QUEUE_HPP

#include <optimized_vector.hpp>
#include <vector>

namespace data_structure {
    template<typename T, class Container = optimized_vector<T>>
    class ArrayQueue {
    public:
        using size_type = typename Container::size_type;
        using difference_type = typename Container::difference_type;
        using value_type = typename Container::value_type;
        using reference = typename Container::reference;
        using pointer = typename Container::pointer;
        using iterator = typename Container::iterator;
        using const_iterator = typename Container::const_iterator;

    private:
        Container container{};

    public:
        size_type size() const {
            return container.size();
        }

        iterator begin() {
            return container.begin();
        }

        iterator end() {
            return container.end();
        }

        inline const_iterator begin() const noexcept {
            return container.begin();
        }

        inline const_iterator end() const noexcept {
            return container.end();
        }

        const_iterator cbegin() const {
            return container.cbegin();
        }

        const_iterator cend() const {
            return container.cend();
        }
    };
};

#endif //DATA_STRUCTURE_FOR_LOVE_ARRAY_QUEUE_HPP
