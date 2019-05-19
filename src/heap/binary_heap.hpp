//
// Created by schrodinger on 4/2/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BINARY_HEAP_HPP
#define DATA_STRUCTURE_FOR_LOVE_BINARY_HEAP_HPP

#include <random>
#include <heap_base.hpp>
#include <chrono>
#include <cassert>

namespace data_structure {
    template<typename T>
    struct BinaryHeapNode {
        BinaryHeapNode *father = nullptr, *left = nullptr, *right = nullptr;
        size_t size = 0;
        T value;

        explicit BinaryHeapNode(const T &t) : value(t) {};

        template<typename ...Args>
        explicit BinaryHeapNode(Args &&...args): value(std::forward<Args>(args)...) {};
    };

    template<typename T,
            typename Compare = std::less<T>,
            typename Alloc = std::allocator<BinaryHeapNode<T>>>
    class BinaryHeap : public Heap<T, Compare> {
        constexpr static Compare compare{};

        Alloc alloc{};
        BinaryHeapNode<T> *root = nullptr;

        void destroy(BinaryHeapNode<T> *node);

    public:
        ~BinaryHeap();

        void push(const T &t) override;

#ifdef DEBUG
        void debug(BinaryHeapNode<T>* m, const std::string& t = "    ") {
            if (m) {
                std::cout << t  << "value: " << m->value <<", size: " << m->size ;

                if(m->father) std::cout << ", father: "<< m->father->value << std::endl;
                else std::cout << std::endl;
                debug(m->left, "    " + t);
                debug(m->right, "    " + t);
            }
        }


        void debug() {
            debug(root);
        }
#endif //DEBUG

        template<typename ...Args>
        void emplace(Args &&...args);

        size_t size() const override;

        const T &top() override;

        void pop() override;

        bool empty() override;
    };

    template<typename T, typename Compare, typename Alloc>
    void BinaryHeap<T, Compare, Alloc>::destroy(BinaryHeapNode<T> *node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        alloc.destroy(node);
        alloc.deallocate(node, 1);
    }

    template<typename T, typename Compare, typename Alloc>
    BinaryHeap<T, Compare, Alloc>::~BinaryHeap() {
        destroy(root);
    }

    template<typename T, typename Compare, typename Alloc>
    void BinaryHeap<T, Compare, Alloc>::push(const T &t) {
        BinaryHeapNode<T> **ptr = &root, *old = nullptr;
        while (*ptr) {
            old = *ptr;
            if ((old->size - (old->left ? old->left->size : 0)) >= (old->left ? old->left->size : 0)) {
                ptr = &old->left;
            } else {
                ptr = &old->right;
            }
            ++old->size;
        }
        *ptr = alloc.allocate(1);
        alloc.construct(*ptr, t);
        (*ptr)->father = old;
        old = *ptr;
        old->size = 1;
        while (old->father && compare(old->value, old->father->value)) {
            std::swap(old->father->size, old->size);
            if (old->father->father) {
                if (old->father->father->left == old->father) {
                    old->father->father->left = old;
                } else {
                    old->father->father->right = old;
                }
            }
            if (old == old->father->left) {
                if (old->father->right) {
                    old->father->right->father = old;
                }
                if (old->left) {
                    old->left->father = old->father;
                }
                if (old->right) {
                    old->right->father = old->father;
                }
                std::swap(old->right, old->father->right);
                old->father->left = old->left;
                old->left = old->father;
                old->father = old->left->father;
                old->left->father = old;
            } else {
                if (old->father->left) {
                    old->father->left->father = old;
                }
                if (old->left) {
                    old->left->father = old->father;
                }
                if (old->right) {
                    old->right->father = old->father;
                }
                std::swap(old->left, old->father->left);
                old->father->right = old->right;
                old->right = old->father;
                old->father = old->right->father;
                old->right->father = old;
            }
            if (!old->father) root = old;
        }
    }

    template<typename T, typename Compare, typename Alloc>
    template<typename... Args>
    void BinaryHeap<T, Compare, Alloc>::emplace(Args &&... args) {
        BinaryHeapNode<T> **ptr = &root, *old = nullptr;
        while (*ptr) {
            old = *ptr;
            if ((old->size - (old->left ? old->left->size : 0)) >= (old->left ? old->left->size : 0)) {
                ptr = &old->left;
            } else {
                ptr = &old->right;
            }
            ++old->size;
        }
        *ptr = alloc.allocate(1);
        alloc.construct(*ptr, std::forward<Args>(args)...);
        (*ptr)->father = old;
        old = *ptr;
        old->size = 1;
        while (old->father && compare(old->value, old->father->value)) {
            std::swap(old->father->size, old->size);
            if (old->father->father) {
                if (old->father->father->left == old->father) {
                    old->father->father->left = old;
                } else {
                    old->father->father->right = old;
                }
            }
            if (old == old->father->left) {
                if (old->father->right) {
                    old->father->right->father = old;
                }
                if (old->left) {
                    old->left->father = old->father;
                }
                if (old->right) {
                    old->right->father = old->father;
                }
                std::swap(old->right, old->father->right);
                old->father->left = old->left;
                old->left = old->father;
                old->father = old->left->father;
                old->left->father = old;
            } else {
                if (old->father->left) {
                    old->father->left->father = old;
                }
                if (old->left) {
                    old->left->father = old->father;
                }
                if (old->right) {
                    old->right->father = old->father;
                }
                std::swap(old->left, old->father->left);
                old->father->right = old->right;
                old->right = old->father;
                old->father = old->right->father;
                old->right->father = old;
            }
            if (!old->father) root = old;
        }
    }

    template<typename T, typename Compare, typename Alloc>
    size_t BinaryHeap<T, Compare, Alloc>::size() const { return root ? root->size : 0; }

    template<typename T, typename Compare, typename Alloc>
    const T &BinaryHeap<T, Compare, Alloc>::top() { return root->value; }

    template<typename T, typename Compare, typename Alloc>
    void BinaryHeap<T, Compare, Alloc>::pop() {
        if (!root) return;
        auto p = root;
        while (p->left || p->right) {
            p->size--;
            if (!p->left || (p->right && !compare(p->left->value, p->right->value))) {
                std::swap(p->value, p->right->value);
                p = p->right;
            } else {
                std::swap(p->value, p->left->value);
                p = p->left;
            }
        }
        if (p->father) {
            if (p == p->father->left) p->father->left = nullptr;
            else p->father->right = nullptr;
        } else {
            root = nullptr;
        }
        this->alloc.destroy(p);
        this->alloc.deallocate(p, 1);
    }

    template<typename T, typename Compare, typename Alloc>
    bool BinaryHeap<T, Compare, Alloc>::empty() {
        return !root;
    }


}

#endif //DATA_STRUCTURE_FOR_LOVE_BINARY_HEAP_HPP
