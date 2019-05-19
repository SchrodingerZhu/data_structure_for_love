//
// Created by schrodinger on 19-4-10.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_FIB_HEAP_HPP
#define DATA_STRUCTURE_FOR_LOVE_FIB_HEAP_HPP

#include <heap_base.hpp>
#include <cstring>

namespace data_structure {
    template<typename T>
    struct FibHeapNode {
        FibHeapNode *next{}, *prev{}, *parent{}, *children{};
        std::size_t degree{};
        std::size_t mark{};
        T value;

        explicit FibHeapNode(const T &t) : value(t) { next = prev = this; }

        template<typename ...Args>
        explicit FibHeapNode(Args &&...args): value(std::forward<Args>(args)...) { next = prev = this; };
    };

    template<typename T,
            typename Compare = std::less<T>>
    class FibHeap : public Heap<T, Compare> {
        using Node = FibHeapNode<T>;
        Node *root = nullptr;
        Node *min_tree = nullptr;
        std::size_t _size{};
        constexpr static Compare compare{};

        void destroy_tree(Node *x);

        Node *concat(Node *first1, Node *first2);

        Node *append(Node *first, Node *x);

        Node *remove_node(Node *first, Node *x);

        void add_tree(Node *t);

        void insert(Node *x);

        Node *link(Node *x, Node *y);

        size_t max_degree(size_t n);

        void consolidate();

        void cut(Node *x);

        void cascading_cut(Node *x);


    public:
        class value_holder;

        bool is_empty();

        ~FibHeap();

        FibHeap() = default;

        FibHeap(const std::initializer_list<T> &list);

        FibHeap(FibHeap &&that) noexcept;

        FibHeap &operator=(FibHeap &&that) noexcept;

        FibHeap(const FibHeap &that) = delete;

        FibHeap &operator=(const FibHeap &that) = delete;

        void push(const T &t) override;

        template<typename ...Args>
        void emplace(Args &&...args);

        value_holder push_and_hold(const T &t);

        template<typename ...Args>
        value_holder emplace_and_hold(Args &&...args);

        const T &top() override;

        void merge(FibHeap &that);

        void pop() override;

        std::size_t size() const override;

        bool empty() override;

    };

    template<typename T, typename Compare>
    void FibHeap<T, Compare>::destroy_tree(FibHeap::Node *x) {
        if (!x) return;
        Node *y{}, *z{};
        if (x->children) {
            y = x->children;
            do {
                z = y;
                y = y->next;
                destroy_tree(z);
            } while (y != x->children);
        }
        delete x;
    }

    template<typename T, typename Compare>
    typename FibHeap<T, Compare>::Node *FibHeap<T, Compare>::concat(FibHeap::Node *first1, FibHeap::Node *first2) {
        Node *last1 = first1->prev;
        Node *last2 = first2->prev;
        last1->next = first2;
        first2->prev = last1;
        last2->next = first1;
        first1->prev = last2;
        return first1;
    }

    template<typename T, typename Compare>
    typename FibHeap<T, Compare>::Node *FibHeap<T, Compare>::append(FibHeap::Node *first, FibHeap::Node *x) {
        if (!first) {
            return x;
        }
        Node *last = first->prev;
        last->next = x;
        x->prev = last;
        x->next = first;
        first->prev = x;
        return first;
    }

    template<typename T, typename Compare>
    typename FibHeap<T, Compare>::Node *FibHeap<T, Compare>::remove_node(FibHeap::Node *first, FibHeap::Node *x) {
        Node *p, *n;
        if (x->next == x && first == x)
            first = nullptr;
        else {
            p = x->prev;
            n = x->next;
            p->next = n;
            n->prev = p;
            x->next = x;
            x->prev = x;
            if (x == first)
                first = n;
        }
        return first;
    }

    template<typename T, typename Compare>
    void FibHeap<T, Compare>::add_tree(FibHeap::Node *t) {
        root = append(root, t);
    }

    template<typename T, typename Compare>
    void FibHeap<T, Compare>::insert(FibHeap::Node *x) {
        add_tree(x);
        if (!min_tree || compare(x->value, min_tree->value))
            min_tree = x;
        _size++;
    }

    template<typename T, typename Compare>
    typename FibHeap<T, Compare>::Node *FibHeap<T, Compare>::link(FibHeap::Node *x, FibHeap::Node *y) {
        if (compare(y->value, x->value)) std::swap(x, y);
        x->children = append(x->children, y);
        y->parent = x;
        x->degree++;
        y->mark = 0;
        return x;
    }

    template<typename T, typename Compare>
    size_t FibHeap<T, Compare>::max_degree(size_t n) {
        size_t k{}, F{}, F2{}, F1 = 1;
        for (F = F1 + F2, k = 2; F < n; ++k) {
            F2 = F1;
            F1 = F;
            F = F1 + F2;
        }
        return k - 2;
    }

    template<typename T, typename Compare>
    void FibHeap<T, Compare>::consolidate() {
        if (!this->root)
            return;
        size_t D = max_degree(this->_size) + 1;
        Node *x, *y;
        Node **a = new Node *[D + 1];
        size_t i, d;
        std::fill(a, a + D + 1, nullptr);
        while (this->root) {
            x = this->root;
            this->root = remove_node(this->root, x);
            d = x->degree;
            while (a[d]) {
                y = a[d];
                x = link(x, y);
                a[d++] = nullptr;
            }
            a[d] = x;
        }
        this->min_tree = this->root = nullptr;
        for (i = 0; i <= D; ++i)
            if (a[i]) {
                this->root = append(this->root, a[i]);
                if (this->min_tree == nullptr || compare(a[i]->value, this->min_tree->value))
                    this->min_tree = a[i];
            }
        delete[] a;
    }

    template<typename T, typename Compare>
    void FibHeap<T, Compare>::cut(FibHeap::Node *x) {
        Node *p = x->parent;
        p->children = remove_node(p->children, x);
        p->degree--;
        root = append(root, x);
        x->parent = nullptr;
        x->mark = 0;
    }

    template<typename T, typename Compare>
    void FibHeap<T, Compare>::cascading_cut(FibHeap::Node *x) {
        Node *p = x->parent;
        if (p) {
            if (!x->mark)
                x->mark = 1;
            else {
                cut(x);
                cascading_cut(p);
            }
        }
    }

    template<typename T, typename Compare>
    class FibHeap<T, Compare>::value_holder {
        Node *node;
        FibHeap *heap;
    public:
        value_holder(Node *node, FibHeap *heap) : node(node), heap(heap) {}

        const auto &get() {
            return node->value;
        }

        void re_push(const T &value) {
            node->value = value;
            auto p = node->parent;
            if (p && compare(value, p->value)) {
                heap->cut(node);
                heap->cascading_cut(p);
            }
            if (compare(value, heap->min_tree->value)) heap->min_tree = node;
        }

        template<typename ...Args>
        void re_emplace(Args &&... args) {
            node->value = T(std::forward<Args>(args)...);
            auto p = node->parent;
            if (p && compare(node->value, p->value)) {
                heap->cut(node);
                heap->cascading_cut(p);
            }
            if (compare(node->value, heap->min_tree->value)) heap->min_tree = node;
        }
    };

    template<typename T, typename Compare>
    bool FibHeap<T, Compare>::is_empty() { return root == nullptr; }

    template<typename T, typename Compare>
    FibHeap<T, Compare>::~FibHeap() {
        Node *x{}, *y{};
        if (root) {
            x = root;
            do {
                y = x;
                x = x->next;
                destroy_tree(y);
            } while (x != root);
        }
    }

    template<typename T, typename Compare>
    FibHeap<T, Compare>::FibHeap(const std::initializer_list<T> &list) {
        for (const auto &i : list) {
            push(i);
        }
    }

    template<typename T, typename Compare>
    FibHeap<T, Compare>::FibHeap(FibHeap &&that) noexcept {
        this->min_tree = that.min_tree;
        this->root = that.root;
        this->_size = that._size;
        that.min_tree = that.root = nullptr;
        that._size = 0;
    }

    template<typename T, typename Compare>
    FibHeap<T, Compare> &FibHeap<T, Compare>::operator=(FibHeap &&that) noexcept {
        this->~FibHeap();
        this->min_tree = that.min_tree;
        this->root = that.root;
        this->_size = that._size;
        that.min_tree = that.root = nullptr;
        that._size = 0;
        return *this;
    }

    template<typename T, typename Compare>
    void FibHeap<T, Compare>::push(const T &t) {
        insert(new Node(t));
    }

    template<typename T, typename Compare>
    template<typename... Args>
    void FibHeap<T, Compare>::emplace(Args &&... args) {
        insert(new Node(std::forward<Args>(args)...));
    }

    template<typename T, typename Compare>
    typename FibHeap<T, Compare>::value_holder FibHeap<T, Compare>::push_and_hold(const T &t) {
        auto m = new Node(t);
        insert(m);
        return {m, this};
    }

    template<typename T, typename Compare>
    template<typename... Args>
    typename FibHeap<T, Compare>::value_holder FibHeap<T, Compare>::emplace_and_hold(Args &&... args) {
        auto m = new Node(std::forward<Args>(args)...);
        insert(m);
        return {m, this};
    }

    template<typename T, typename Compare>
    const T &FibHeap<T, Compare>::top() {
        return min_tree->value;
    }

    template<typename T, typename Compare>
    void FibHeap<T, Compare>::merge(FibHeap &that) {
        if (is_empty()) {
            this->root = that.root;
            this->min_tree = that.min_tree;
            this->_size = that._size;
            that.root = that.min_tree = nullptr;
            that._size = 0;
            return;
        }
        if (that.is_empty()) return;
        root = concat(root, that.root);
        min_tree = compare(min_tree->value, that.min_tree->value) ? min_tree : that.min_tree;
        _size += that._size;
        that.root = that.min_tree = nullptr;
        that._size = 0;
    }

    template<typename T, typename Compare>
    void FibHeap<T, Compare>::pop() {
        Node *x = this->min_tree;
        Node *y, *child;
        if (x) {
            child = x->children;
            if (child) {
                do {
                    y = child;
                    child = child->next;
                    append(this->root, y);
                    y->parent = nullptr;
                } while (child != x->children);
            }
            this->root = remove_node(this->root, x);
            this->_size--;
            consolidate();
            delete (x);
        }
    }

    template<typename T, typename Compare>
    std::size_t FibHeap<T, Compare>::size() const {
        return _size;
    }

    template<typename T, typename Compare>
    bool FibHeap<T, Compare>::empty() {
        return !root;
    }

}
#endif //DATA_STRUCTURE_FOR_LOVE_FIB_HEAP_HPP
