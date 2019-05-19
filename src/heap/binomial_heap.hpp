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
    class BinomialHeap : public Heap<T, Compare> {
        constexpr static Compare compare{};
        BinomialHeapNode<T> *root = nullptr;
        using Node = BinomialHeapNode<T>;
        std::size_t _size = 0;

        Node *link(Node *a, Node *b) noexcept;

        std::pair<Node *, Node *> extract_first(Node *heap) noexcept;

        Node *insert_tree(Node *heap, Node *tree) noexcept;

        inline auto insert(Node *&heap, const T &t) noexcept;

        template<typename ...Args>
        inline Node *emplace(Node *&heap, Args &&... args);

        std::tuple<Node *, Node *, Node *> append_tree(Node *head, Node *prev, Node *tail, Node *x) noexcept;

        std::tuple<Node *, Node *, Node *> append_trees(Node *h, Node *p, Node *t, Node *xs) noexcept;

        Node *merge(Node *h1, Node *h2) noexcept;

        Node *reverse(Node *h);

        std::pair<Node *, Node *> remove_min_tree(Node *h) noexcept;

        const T &top(Node *h) noexcept;

        std::pair<T, Node *> pop(Node *h) noexcept;

        void destroy(Node *h);

        Node *copy(Node *h, Node *father = nullptr);

    public:
        class value_holder;

        void push(const T &t) override;

        value_holder push_and_hold(const T &t);

        const T &top() override;;

        size_t size() const override;;

        void pop() override;;

        template<typename ...Args>
        void emplace(Args &&... args);

        template<typename ...Args>
        value_holder emplace_and_hold(Args &&... args);

        void merge(BinomialHeap &that);;

        BinomialHeap() = default;

        BinomialHeap(const BinomialHeap &that);

        BinomialHeap &operator=(const BinomialHeap &that);

        BinomialHeap(BinomialHeap &&that) noexcept;

        BinomialHeap &operator=(BinomialHeap &&that) noexcept;

        BinomialHeap(const std::initializer_list<T> &list);

        ~BinomialHeap();

        bool empty() override;
    };

    template<typename T, typename Compare>
    typename BinomialHeap<T, Compare>::Node *
    BinomialHeap<T, Compare>::link(BinomialHeap::Node *a, BinomialHeap::Node *b) noexcept {
        if (compare(b->value, a->value)) {
            std::swap(a, b);
        }
        b->sibling = a->child;
        a->child = b;
        b->father = a;
        a->rank++;
        return a;
    }

    template<typename T, typename Compare>
    std::pair<typename BinomialHeap<T, Compare>::Node *, typename BinomialHeap<T, Compare>::Node *>
    BinomialHeap<T, Compare>::extract_first(BinomialHeap::Node *heap) noexcept {
        Node *top = nullptr;
        if (heap) {
            top = heap;
            heap = heap->sibling;
            top->sibling = nullptr;
        }
        return {top, heap};
    }

    template<typename T, typename Compare>
    typename BinomialHeap<T, Compare>::Node *
    BinomialHeap<T, Compare>::insert_tree(BinomialHeap::Node *heap, BinomialHeap::Node *tree) noexcept {
        while (heap and heap->rank == tree->rank) {
            auto res = extract_first(heap);
            heap = res.second;
            auto t1 = res.first;
            tree = link(tree, t1);
        }
        tree->sibling = heap;
        return tree;
    }

    template<typename T, typename Compare>
    std::tuple<typename BinomialHeap<T, Compare>::Node *, typename BinomialHeap<T, Compare>::Node *, typename BinomialHeap<T, Compare>::Node *>
    BinomialHeap<T, Compare>::append_tree(BinomialHeap::Node *head, BinomialHeap::Node *prev, BinomialHeap::Node *tail,
                                          BinomialHeap::Node *x) noexcept {
        if (!head) return {x, nullptr, x};
        if (tail->rank == x->rank) {
            tail = link(tail, x);
            if (!prev) return {tail, nullptr, tail};
            prev->sibling = tail;
        } else {
            tail->sibling = x;
            prev = tail;
            tail = x;
        }
        return {head, prev, tail};
    }

    template<typename T, typename Compare>
    auto BinomialHeap<T, Compare>::insert(BinomialHeap::Node *&heap, const T &t) noexcept {
        auto temp = new Node(t);
        return insert_tree(heap, temp);
    }

    template<typename T, typename Compare>
    template<typename... Args>
    typename BinomialHeap<T, Compare>::Node *
    BinomialHeap<T, Compare>::emplace(BinomialHeap::Node *&heap, Args &&... args) {
        auto temp = new Node(std::forward<Args>(args)...);
        return insert_tree(heap, temp);
    }

    template<typename T, typename Compare>
    std::tuple<typename BinomialHeap<T, Compare>::Node *, typename BinomialHeap<T, Compare>::Node *, typename BinomialHeap<T, Compare>::Node *>
    BinomialHeap<T, Compare>::append_trees(BinomialHeap::Node *h, BinomialHeap::Node *p, BinomialHeap::Node *t,
                                           BinomialHeap::Node *xs) noexcept {
        while (xs) {
            auto temp = extract_first(xs);
            xs = temp.second;
            auto temp2 = append_tree(h, p, t, temp.first);
            h = std::get<0>(temp2);
            p = std::get<1>(temp2);
            t = std::get<2>(temp2);
        }
        return {h, p, t};
    }

    template<typename T, typename Compare>
    typename BinomialHeap<T, Compare>::Node *
    BinomialHeap<T, Compare>::merge(BinomialHeap::Node *h1, BinomialHeap::Node *h2) noexcept {
        if (!h1) return h2;
        if (!h2) return h1;
        Node *h{}, *p{}, *t{};
        while (h1 && h2) {
            Node *x = nullptr;
            if (h1->rank < h2->rank) {
                auto temp = extract_first(h1);
                x = temp.first;
                h1 = temp.second;
            } else if (h2->rank < h1->rank) {
                auto temp = extract_first(h2);
                x = temp.first;
                h2 = temp.second;
            } else {
                auto temp1 = extract_first(h1),
                        temp2 = extract_first(h2);
                h1 = temp1.second;
                h2 = temp2.second;
                x = link(temp1.first, temp2.first);
            }
            auto temp = append_trees(h, p, t, x);
            h = std::get<0>(temp);
            p = std::get<1>(temp);
            t = std::get<2>(temp);
        }
        if (h1) {
            auto temp = append_trees(h, p, t, h1);
            h = std::get<0>(temp);
            p = std::get<1>(temp);
            t = std::get<2>(temp);
        }
        if (h2) {
            auto temp = append_trees(h, p, t, h2);
            h = std::get<0>(temp);
        }
        return h;
    }

    template<typename T, typename Compare>
    typename BinomialHeap<T, Compare>::BinomialHeap::Node *BinomialHeap<T, Compare>::reverse(BinomialHeap::Node *h) {
        Node *prev = nullptr;
        while (h) {
            auto x = h;
            h = h->sibling;
            x->sibling = prev;
            prev = x;
        }
        return prev;
    }

    template<typename T, typename Compare>
    std::pair<typename BinomialHeap<T, Compare>::Node *, typename BinomialHeap<T, Compare>::Node *>
    BinomialHeap<T, Compare>::remove_min_tree(BinomialHeap::Node *h) noexcept {
        Node *head = h,
                *prev_min = nullptr,
                *min_t = nullptr,
                *prev = nullptr;
        while (h) {
            if (!min_t || compare(h->value, min_t->value)) {
                min_t = h;
                prev_min = prev;
            }
            prev = h;
            h = h->sibling;
        }
        if (prev_min) {
            prev_min->sibling = min_t->sibling;
        } else {
            head = min_t->sibling;
        }
        min_t->sibling = nullptr;
        return {min_t, head};
    }

    template<typename T, typename Compare>
    const T &BinomialHeap<T, Compare>::top(BinomialHeap::Node *h) noexcept {
        Node *min_t = nullptr;
        while (h) {
            if (!min_t || compare(h->value, min_t->value)) min_t = h;
            h = h->sibling;
        }
        return min_t->value;
    }

    template<typename T, typename Compare>
    std::pair<T, typename BinomialHeap<T, Compare>::Node *>
    BinomialHeap<T, Compare>::pop(BinomialHeap::Node *h) noexcept {
        auto temp = remove_min_tree(h);
        h = merge(temp.second, reverse(temp.first->child));
        temp.first->child = nullptr;
        std::pair<T, Node *> res = {temp.first->value, h};
        delete (temp.first);
        return res;
    }

    template<typename T, typename Compare>
    void BinomialHeap<T, Compare>::destroy(BinomialHeap::Node *h) {
        if (!h) return;
        else {
            destroy(h->sibling);
            destroy(h->child);
            delete (h);
        }
    }

    template<typename T, typename Compare>
    typename BinomialHeap<T, Compare>::Node *
    BinomialHeap<T, Compare>::copy(BinomialHeap::Node *h, BinomialHeap::Node *father) {
        if (!h)
            return nullptr;
        auto m = new Node(h->value);
        m->rank = h->rank;
        m->father = father;
        m->sibling = copy(h->sibling, father);
        m->child = copy(h->child, h);
        return m;
    }

    template<typename T, typename Compare>
    class BinomialHeap<T, Compare>::value_holder {
        Node *node;
    public:
        value_holder(Node *node) : node(node) {}

        const auto &get() {
            return node->value;
        }

        void re_push(const T &value) {
            node->value = value;
            auto p = node->father;
            while (p && compare(node->value, p->value)) {
                std::swap(node->value, p->value);
                node = p;
                p = p->father;
            }
        }

        template<typename ...Args>
        void re_emplace(Args &&... args) {
            node->value = T(std::forward<Args>(args)...);
            auto p = node->father;
            while (p && compare(node->value, p->value)) {
                std::swap(node->value, p->value);
                node = p;
                p = p->father;
            }
        }
    };

    template<typename T, typename Compare>
    void BinomialHeap<T, Compare>::push(const T &t) {
        root = insert(root, t);
        _size++;
    }

    template<typename T, typename Compare>
    typename BinomialHeap<T, Compare>::value_holder BinomialHeap<T, Compare>::push_and_hold(const T &t) {
        _size++;
        auto temp = new Node(t);
        root = insert_tree(root, temp);
        return {temp};
    }

    template<typename T, typename Compare>
    const T &BinomialHeap<T, Compare>::top() {
        return top(root);
    }

    template<typename T, typename Compare>
    size_t BinomialHeap<T, Compare>::size() const {
        return _size;
    }

    template<typename T, typename Compare>
    void BinomialHeap<T, Compare>::pop() {
        root = pop(root).second;
        _size--;
    }

    template<typename T, typename Compare>
    template<typename... Args>
    void BinomialHeap<T, Compare>::emplace(Args &&... args) {
        root = emplace(root, std::forward<Args>(args)...);
        _size++;
    }

    template<typename T, typename Compare>
    template<typename... Args>
    typename BinomialHeap<T, Compare>::value_holder BinomialHeap<T, Compare>::emplace_and_hold(Args &&... args) {
        _size++;
        auto temp = new Node(std::forward<Args>(args)...);
        root = insert_tree(root, temp);
        return {temp};
    }

    template<typename T, typename Compare>
    void BinomialHeap<T, Compare>::merge(BinomialHeap &that) {
        root = merge(root, that.root);
        that.root = nullptr;
        _size += that.size();
    }

    template<typename T, typename Compare>
    BinomialHeap<T, Compare>::BinomialHeap(const BinomialHeap &that) {
        root = copy(that.root);
        _size = that.size();
    }

    template<typename T, typename Compare>
    BinomialHeap<T, Compare> &BinomialHeap<T, Compare>::operator=(const BinomialHeap &that) {
        destroy(root);
        root = copy(that.root);
        _size = that.size();
        return *this;
    }

    template<typename T, typename Compare>
    BinomialHeap<T, Compare>::BinomialHeap(BinomialHeap &&that) noexcept {
        root = that.root;
        that.root = nullptr;
        _size = that.size();
        that._size = 0;
    }

    template<typename T, typename Compare>
    BinomialHeap<T, Compare> &BinomialHeap<T, Compare>::operator=(BinomialHeap &&that) noexcept {
        destroy(root);
        root = that.root;
        that.root = nullptr;
        _size = that.size();
        that._size = 0;
        return *this;
    }

    template<typename T, typename Compare>
    BinomialHeap<T, Compare>::BinomialHeap(const std::initializer_list<T> &list) {
        for (const auto &i: list) {
            this->push(i);
        }
    }

    template<typename T, typename Compare>
    BinomialHeap<T, Compare>::~BinomialHeap() {
        destroy(root);
    }

    template<typename T, typename Compare>
    bool BinomialHeap<T, Compare>::empty() {
        return !root;
    }
}
#endif //DATA_STRUCTURE_FOR_LOVE_BINOMIAL_HEAP_HPP
