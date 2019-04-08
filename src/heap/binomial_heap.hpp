//
// Created by schrodinger on 19-4-9.
//
#include <heap_base.hpp>

#ifndef DATA_STRUCTURE_FOR_LOVE_BINOMIAL_HEAP_HPP
#define DATA_STRUCTURE_FOR_LOVE_BINOMIAL_HEAP_HPP
namespace data_structure {
    template<typename T>
    struct BinomialHeapNode {
        BinomialHeapNode *father = nullptr,
                *child = nullptr, *sibling = nullptr;
        T value;
        std::size_t rank{};

        explicit BinomialHeapNode(const T &t) : value(t) {}

        template<typename ...Args>
        explicit BinomialHeapNode(Args &&...args): value(std::forward<Args>(args)...) {};
    };

    template<typename T,
            typename Compare = std::less<T>>
    class BinomialHeap : public MergeHeap<T, Compare> {
        constexpr static Compare compare{};
        BinomialHeapNode<T> *root = nullptr;
        using Node = BinomialHeapNode<T>;

        Node *link(Node *a, Node *b) {
            if (compare(a->value, b->value)) {
                std::swap(a, b);
            }
            b->sibling = a->child;
            a->child = b;
            b->father = a;
            a->rank++;
            return a;
        }

        void insert_tree(Node *tree) {

        }
    };
}
#endif //DATA_STRUCTURE_FOR_LOVE_BINOMIAL_HEAP_HPP
