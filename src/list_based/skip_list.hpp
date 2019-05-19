//
// Created by schrodinger on 19-5-20.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_SKIP_LIST_HPP
#define DATA_STRUCTURE_FOR_LOVE_SKIP_LIST_HPP

#include <functional>
#include <static_random_helper.hpp>
#include <cstring>
#include <iostream>
#include <compare.hpp>

namespace data_structure {

    struct SKNode {
        SKNode **prev = nullptr;
        SKNode **next = nullptr;
        size_t height = 0;

        explicit SKNode(size_t height) noexcept : height(height) {
            if (height) {
                prev = new SKNode *[height];
                next = new SKNode *[height];
                std::memset(prev, 0, sizeof(SKNode *) * height);
                std::memset(next, 0, sizeof(SKNode *) * height);
            }
        }

        void grow(size_t new_height) noexcept {
            auto new_prev = new SKNode *[new_height];
            auto new_next = new SKNode *[new_height];
            std::memset(new_prev, 0, sizeof(SKNode *) * new_height);
            std::memset(new_next, 0, sizeof(SKNode *) * new_height);
            if (height) {
                std::memcpy(new_prev, prev, sizeof(SKNode *) * height);
                std::memcpy(new_next, next, sizeof(SKNode *) * height);
            }
            height = new_height;
            if (prev) delete[](prev);
            if (next) delete[](next);
            prev = new_prev;
            next = new_next;
        }

        ~SKNode() {
            if (prev) delete[](prev);
            if (next) delete[](next);
        }

    };

    template<class T>
    struct SkipNode : SKNode {
        T value;

        template<typename ...Args>
        explicit SkipNode(size_t height, Args &&...args) : SKNode(height), value(std::forward<Args>(args)...) {
        }
    };

    template<class T, class Compare = utils::DefaultCompare<T>, class Alloc = std::allocator<SkipNode<T>>>
    class SkipList {
    public:
        Alloc alloc{};
        constexpr static Compare compare{};
        using Node = SkipNode<T>;
        utils::GeoIntGen<size_t> rand;
        SKNode sentinel{0};
        size_t height = 0, _size = 0;

        SKNode *find_prev(const T &x);

        SKNode **find_prev(const T &x, size_t target_height);

    public:
        class iterator;

        void insert(const T &x);

        void erase(const T &x);

        iterator begin();

        iterator end();

        std::optional<T> succ(const T &x);

        std::optional<T> pred(const T &x);

        bool contains(const T &x);

        void display();

        ~SkipList();
    };

    template<class T, class Compare, class Alloc>
    SKNode *SkipList<T, Compare, Alloc>::find_prev(const T &x) {
        if (_size) {
            size_t current = height;
            SKNode *node = &sentinel;
            while (current) {
                if (node != &sentinel && compare(static_cast<Node *>(node)->value, x) == utils::Eq) {
                    return node;
                }
                if (node->next[current - 1] &&
                    compare(static_cast<Node *>(node->next[current - 1])->value, x) != utils::Greater) {
                    node = node->next[current - 1];
                } else {
                    current--;
                }
            }
            return node;
        }
        return &sentinel;
    }

    template<class T, class Compare, class Alloc>
    SKNode **SkipList<T, Compare, Alloc>::find_prev(const T &x, size_t target_height) {
        auto arrays = new SKNode *[target_height];
        std::memset(arrays, 0, sizeof(SKNode *) * target_height);
        if (_size) {
            size_t current = height;
            SKNode *node = &sentinel;
            while (current) {
                if (node->next[current - 1] &&
                    compare(static_cast<Node *>(node->next[current - 1])->value, x) == utils::Less) {
                    node = node->next[current - 1];
                } else {
                    if (--current < target_height) arrays[current] = node;
                }
            }
        }
        while (target_height--) {
            if (!arrays[target_height]) arrays[target_height] = &sentinel;
            else break;
        }
        return arrays;
    }

    template<class T, class Compare, class Alloc>
    void SkipList<T, Compare, Alloc>::insert(const T &x) {
        auto target_height = rand() + 1;
        auto array = find_prev(x, target_height);
        auto node = alloc.allocate(1);
        alloc.construct(node, target_height, x);
        if (target_height > height) {
            height = target_height;
            sentinel.grow(height);
        }
        while (target_height--) {
            node->next[target_height] = array[target_height]->next[target_height];
            array[target_height]->next[target_height] = node;
            if (node->next[target_height]) node->next[target_height]->prev[target_height] = node;
            node->prev[target_height] = array[target_height];
        }
        delete[](array);
        _size++;
    }

    template<class T, class Compare, class Alloc>
    class SkipList<T, Compare, Alloc>::iterator {
        SkipNode<T> *node;
    public:
        explicit iterator(SkipNode<T> *node) : node(node) {}

        bool operator!=(const iterator &that) const {
            return that.node != node;
        }

        bool operator==(const iterator &that) const {
            return that.node == node;
        }

        iterator &operator++() {
            node = static_cast<Node *>(node->next[0]);
            return *this;
        }

        const iterator operator++(int) {
            auto m = *this;
            node = static_cast<Node *>(node->next[0]);
            return m;
        }

        T &operator*() {
            return node->value;
        }

        T *operator->() {
            return &node->value;
        }
    };

    template<class T, class Compare, class Alloc>
    typename SkipList<T, Compare, Alloc>::iterator SkipList<T, Compare, Alloc>::begin() {
        if (_size) {
            return iterator(static_cast<Node *>(sentinel.next[0]));
        } else {
            return iterator(nullptr);
        }
    }

    template<class T, class Compare, class Alloc>
    typename SkipList<T, Compare, Alloc>::iterator SkipList<T, Compare, Alloc>::end() {
        return iterator(nullptr);
    }

    template<class T, class Compare, class Alloc>
    void SkipList<T, Compare, Alloc>::display() {
        size_t t = height;
        while (t--) {
            auto node = static_cast<Node *>(sentinel.next[t]);
            std::cout << t << ": ";
            while (node) {
                std::cout << node->value << " ";
                node = static_cast<Node *>(node->next[t]);
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    template<class T, class Compare, class Alloc>
    SkipList<T, Compare, Alloc>::~SkipList() {
        if (_size) {
            auto node = sentinel.next[0];
            SKNode *p = nullptr;
            while (node) {
                p = node->next[0];
                alloc.destroy(static_cast<Node *>(node));
                alloc.deallocate(static_cast<Node *>(node), 1);
                node = p;
            }
        }
    }

    template<class T, class Compare, class Alloc>
    bool SkipList<T, Compare, Alloc>::contains(const T &x) {
        auto node = find_prev(x);
        return (node != &sentinel && compare(static_cast<Node *>(node)->value, x) == utils::Eq);
    }

    template<class T, class Compare, class Alloc>
    void SkipList<T, Compare, Alloc>::erase(const T &x) {
        auto node = find_prev(x);
        if (node == &sentinel || compare(static_cast<Node *>(node)->value, x) != utils::Eq) return;
        else {
            for (auto i = node->height; i > 0; --i) {
                node->prev[i - 1]->next[i - 1] = node->next[i - 1];
                if (node->next[i - 1]) node->next[i - 1]->prev[i - 1] = node->prev[i - 1];
            }
            alloc.destroy(static_cast<Node *>(node));
            alloc.deallocate(static_cast<Node *>(node), 1);
            _size--;
        }
    }

    template<class T, class Compare, class Alloc>
    std::optional<T> SkipList<T, Compare, Alloc>::succ(const T &x) {
        size_t current = height;
        SKNode *node = &sentinel;
        while (current) {
            if (node->next[current - 1] &&
                compare(static_cast<Node *>(node->next[current - 1])->value, x) != utils::Greater) {
                node = node->next[current - 1];
            } else {
                current--;
            }
        }
        if (node->next[0])
            return {static_cast<Node *>(node->next[0])->value};
        else return std::nullopt;
    }

    template<class T, class Compare, class Alloc>
    std::optional<T> SkipList<T, Compare, Alloc>::pred(const T &x) {
        size_t current = height;
        SKNode *node = &sentinel;
        while (current) {
            if (node->next[current - 1] &&
                compare(static_cast<Node *>(node->next[current - 1])->value, x) == utils::Less) {
                node = node->next[current - 1];
            } else {
                current--;
            }
        }
        if (node != &sentinel)
            return {static_cast<Node *>(node)->value};
        else return std::nullopt;
    }

}
#endif //DATA_STRUCTURE_FOR_LOVE_SKIP_LIST_HPP
