//
// Created by schrodinger on 2/18/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_TEST_ROOTISH_STACK_HPP
#define DATA_STRUCTURE_FOR_LOVE_TEST_ROOTISH_STACK_HPP

#include <optimized_vector.hpp>
#include <object_management.hpp>
#include <vector>
#include <cmath>
#include <functional>
#include <new>
namespace data_structure {

    struct NormalSqrt {
        inline double operator()(double x) const noexcept {
            return sqrt(x);
        }
    };

    struct InverseSqrt {
    private:
        inline double __inv(double x) const noexcept {
            double x_half = 0.5f * x;
            long i;
            std::memcpy(&i, &x, sizeof(x));
            i = 0x5fe6ec85e7de30da - (i >> 1u);
            x = *(double *) &i;
            x = x * (1.5f - x_half * x * x);
            return x;
        }

    public:
        inline double operator()(double x) const noexcept {
            auto res = 1.0 / __inv(x);
            res = (res + (x / res)) / 2.0;
            res = (res + (x / res)) / 2.0;
            res = (res + (x / res)) / 2.0;
            return res;
        }
    };

    template<typename T, class PtrContainer = std::vector<T *>, class Sqrt = NormalSqrt>
    class RootishStack {
    private:
        size_t n = 0;
        PtrContainer container{};

        constexpr static size_t i2b(size_t i) noexcept;

        constexpr static Sqrt __sqrt{};

        void grow() noexcept;

        void shrink() noexcept;

        T *locate(size_t index) noexcept;

    public:
        size_t size() const noexcept;

        void push_back(const T &i);

        void clear() noexcept;

        T &operator[](size_t index) noexcept;

        class iterator;

        friend class iterator;

        iterator begin() noexcept;

        iterator end() noexcept;

        void erase(size_t index);

        ~RootishStack();
    };

    template<typename T, class PtrContainer, class Sqrt>
    class RootishStack<T, PtrContainer, Sqrt>::iterator {
        size_t block, pos;
        RootishStack *stack;

        inline T *cast() noexcept { return stack->container[block] + pos; }

    public:
        using value_type = T;
        using iterator_category = std::bidirectional_iterator_tag;
        using pointer = T *;
        using reference = T &;
        using difference_type = std::ptrdiff_t;

        iterator(size_t block, size_t pos, RootishStack *stack) : block(block), pos(pos), stack(stack) {}

        T &operator*() {
            return stack->container[block][pos];
        }

        T *operator->() {
            return stack->container[block] + pos;
        }

        iterator &operator++() {
            if (pos < block) pos++;
            else {
                block++;
                pos = 0;
            }
            return *this;
        }

        const iterator operator++(int) {
            auto copy = *this;
            if (pos < block) pos++;
            else {
                block++;
                pos = 0;
            }
            return copy;
        }

        iterator &operator--() {
            if (pos) pos--;
            else {
                pos = --block;
            }
            return *this;
        }

        const iterator operator--(int) {
            auto copy = *this;
            if (pos) pos--;
            else {
                pos = --block;
            }
            return copy;
        }

        bool operator<(const iterator &that) {
            return block < that.block || (block == that.block && pos < that.pos);
        }

        bool operator==(const iterator &that) {
            return stack == that.stack && block == that.block && pos == that.pos;
        }

        bool operator!=(const iterator &that) {
            return stack != that.stack || block != that.block || pos != that.pos;
        }

        friend RootishStack;
    };

    template<typename T, class PtrContainer, class Sqrt>
    constexpr size_t RootishStack<T, PtrContainer, Sqrt>::i2b(size_t i) noexcept {
        return ceil((-3.0 + __sqrt(9 + 8 * i)) / 2.0);
    }

    template<typename T, class PtrContainer, class Sqrt>
    void RootishStack<T, PtrContainer, Sqrt>::grow() noexcept {
        container.push_back(static_cast<T *>(::operator new(sizeof(T) * (container.size() + 1))));
    }

    template<typename T, class PtrContainer, class Sqrt>
    void RootishStack<T, PtrContainer, Sqrt>::shrink() noexcept {
        auto r = container.size();
        while (r > 0 && ((r - 2) * (r - 1) >> 1) >= n) {
            ::operator delete(container.back());
            container.resize(--r);
        }
    }

    template<typename T, class PtrContainer, class Sqrt>
    void RootishStack<T, PtrContainer, Sqrt>::push_back(const T &i) {
        auto r = container.size();
        if (((r * (r + 1)) >> 1) < n + 1) grow();
        utils::emplace_construct(locate(n++), i);
    }

    template<typename T, class PtrContainer, class Sqrt>
    T *RootishStack<T, PtrContainer, Sqrt>::locate(size_t index) noexcept {
        auto b = i2b(index);
        auto j = index - ((b * (b + 1)) >> 1u);
        return container[b] + j;
    }

    template<typename T, class PtrContainer, class Sqrt>
    T &RootishStack<T, PtrContainer, Sqrt>::operator[](size_t index) noexcept {
        return *locate(index);
    }

    template<typename T, class PtrContainer, class Sqrt>
    void RootishStack<T, PtrContainer, Sqrt>::clear() noexcept {
        for (auto i : container) { ::operator delete(i); }
        container.clear();
        n = 0;
    }

    template<typename T, class PtrContainer, class Sqrt>
    size_t RootishStack<T, PtrContainer, Sqrt>::size() const noexcept {
        return n;
    }

    template<typename T, class PtrContainer, class Sqrt>
    void RootishStack<T, PtrContainer, Sqrt>::erase(size_t index) {
        auto b = i2b(index);
        auto u = index - ((b * (b + 1)) >> 1u);
        auto begin = iterator(b, u, this);
        auto dest = iterator(b, u, this);
        utils::destroy_at(begin.cast());
        std::uninitialized_move(++begin, end(), dest);
        n--;
        auto r = container.size();
        if ((((r - 2) * (r - 1)) >> 1) >= n) shrink();
    }

    template<typename T, class PtrContainer, class Sqrt>
    typename RootishStack<T, PtrContainer, Sqrt>::iterator RootishStack<T, PtrContainer, Sqrt>::begin() noexcept {
        return RootishStack::iterator(0, 0, this);
    }

    template<typename T, class PtrContainer, class Sqrt>
    typename RootishStack<T, PtrContainer, Sqrt>::iterator RootishStack<T, PtrContainer, Sqrt>::end() noexcept {
        return RootishStack::iterator(container.size(), 0, this);
    }

    template<typename T, class PtrContainer, class Sqrt>
    RootishStack<T, PtrContainer, Sqrt>::~RootishStack() {
        for (auto i : container) { ::operator delete(i); }
    }


};

#endif //DATA_STRUCTURE_FOR_LOVE_TEST_ROOTISH_STACK_HPP
