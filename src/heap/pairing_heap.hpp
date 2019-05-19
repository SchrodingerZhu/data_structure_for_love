//
// Created by schrodinger on 19-4-9.
//
#include <heap_base.hpp>

#ifndef DATA_STRUCTURE_FOR_LOVE_PAIRING_HEAP_HPP
#define DATA_STRUCTURE_FOR_LOVE_PAIRING_HEAP_HPP
namespace data_structure {
    template<typename T>
    struct PairingHeapNode {
        PairingHeapNode *next{}, *parent{}, *children{};
        T value;

        explicit PairingHeapNode(const T &t) : value(t) {}

        template<typename ...Args>
        explicit PairingHeapNode(Args &&...args): value(std::forward<Args>(args)...) {};
    };

    template<typename T,
            typename Compare = std::less<T>>
    class PairingHeap : public Heap<T, Compare> {
        using Node = PairingHeapNode<T>;

        constexpr static Compare compare{};

        static void destroy(Node *h) noexcept;

        static Node *copy(Node *node, Node *parent = nullptr);

        static Node *push_front(Node *list, Node *x) noexcept;

        static Node *remove_node(Node *list, Node *x) noexcept;

        static Node *merge(Node *h1, Node *h2) noexcept;

        static Node *push(Node *h, const T &x) noexcept;

        template<typename ...Args>
        static Node *emplace(Node *h, Args &&...args) noexcept;

        static const T &top(Node *h) noexcept;


        static Node *push_decreased_key(Node *h, Node *x, const T &key) noexcept;

        template<typename ...Args>
        static Node *emplace_decreased_key(Node *h, Node *x, Args &&...args) noexcept;

        static Node *pop(Node *h) noexcept;

        Node *root = nullptr;
        size_t _size = 0;
    public:

        class value_holder;

        PairingHeap() = default;

        PairingHeap(const std::initializer_list<T> &list);

        PairingHeap(PairingHeap &&that) noexcept;

        PairingHeap &operator=(PairingHeap &&that) noexcept;

        PairingHeap(const PairingHeap &that);

        PairingHeap &operator=(const PairingHeap &that);;


        ~PairingHeap();

        const T &top() override;

        void push(const T &t) override;

        template<typename ...Args>
        void emplace(Args &&... args) noexcept;

        void merge(PairingHeap &that);

        size_t size() const override;;

        void pop() override;

        bool is_empty();

        value_holder push_and_hold(const T &t);

        template<typename ...Args>
        value_holder emplace_and_hold(Args &&...args);


        bool empty() override;

    };

    template<typename T, typename Compare>
    void PairingHeap<T, Compare>::destroy(PairingHeap::Node *h) noexcept {
        Node *t = nullptr;
        if (h) {
            while ((t = h->children)) {
                h->children = h->children->next;
                destroy(t);
            }
        }
        delete h;
    }

    template<typename T, typename Compare>
    typename PairingHeap<T, Compare>::Node *
    PairingHeap<T, Compare>::copy(PairingHeap::Node *node, PairingHeap::Node *parent) {
        if (!node)
            return nullptr;
        auto p = new Node(node->value);
        p->parent = parent;
        if (node->children) p->children = copy(node->children, p);
        if (node->next) p->next = copy(node->next);
        return p;
    }

    template<typename T, typename Compare>
    typename PairingHeap<T, Compare>::Node *
    PairingHeap<T, Compare>::push_front(PairingHeap::Node *list, PairingHeap::Node *x) noexcept {
        x->next = list;
        return x;
    }

    template<typename T, typename Compare>
    typename PairingHeap<T, Compare>::Node *
    PairingHeap<T, Compare>::remove_node(PairingHeap::Node *list, PairingHeap::Node *x) noexcept {
        auto p = list;
        if (x == list) return x->next;
        while (p && p->next != x) p = p->next;
        if (p) p->next = x->next;
        return list;
    }

    template<typename T, typename Compare>
    typename PairingHeap<T, Compare>::Node *
    PairingHeap<T, Compare>::merge(PairingHeap::Node *h1, PairingHeap::Node *h2) noexcept {
        if (!h1) return h2;
        if (!h2) return h1;
        if (compare(h2->value, h1->value)) std::swap(h1, h2);
        h2->next = h1->children;
        h1->children = h2;
        h2->parent = h1;
        h1->next = nullptr;
        return h1;
    }

    template<typename T, typename Compare>
    typename PairingHeap<T, Compare>::Node *PairingHeap<T, Compare>::push(PairingHeap::Node *h, const T &x) noexcept {
        return merge(h, new Node(x));
    }

    template<typename T, typename Compare>
    template<typename... Args>
    typename PairingHeap<T, Compare>::Node *
    PairingHeap<T, Compare>::emplace(PairingHeap::Node *h, Args &&... args) noexcept {
        return merge(h, new Node(std::forward<Args>(args)...));
    }

    template<typename T, typename Compare>
    const T &PairingHeap<T, Compare>::top(PairingHeap::Node *h) noexcept {
        return h->value;
    }

    template<typename T, typename Compare>
    typename PairingHeap<T, Compare>::Node *
    PairingHeap<T, Compare>::push_decreased_key(PairingHeap::Node *h, PairingHeap::Node *x, const T &key) noexcept {
        x->value = key;
        if (x->parent) {
            x->parent->children = remove_node(x->parent->children, x);
            x->parent = nullptr;
            return merge(h, x);
        }
        return h;
    }

    template<typename T, typename Compare>
    template<typename... Args>
    typename PairingHeap<T, Compare>::Node *
    PairingHeap<T, Compare>::emplace_decreased_key(PairingHeap::Node *h, PairingHeap::Node *x,
                                                   Args &&... args) noexcept {
        x->value = T(std::forward<Args>(args)...);
        if (x->parent) {
            x->parent->children = remove_node(x->parent->children, x);
            x->parent = nullptr;
            return merge(h, x);
        }
        return h;
    }

    template<typename T, typename Compare>
    typename PairingHeap<T, Compare>::Node *PairingHeap<T, Compare>::pop(PairingHeap::Node *h) noexcept {
        Node *x{}, *y{}, *list{};
        while ((x = h->children)) {
            if ((h->children = y = x->next))
                h->children = h->children->next;
            list = push_front(list, merge(x, y));
        }
        x = nullptr;
        while ((y = list)) {
            list = list->next;
            x = merge(x, y);
        }
        delete h;
        return x;
    }

    template<typename T, typename Compare>
    class PairingHeap<T, Compare>::value_holder {
        Node *node;
        PairingHeap *heap;
    public:
        value_holder(Node *node, PairingHeap *heap) : node(node), heap(heap) {}

        const auto &get() {
            return node->value;
        }

        void re_push(const T &value) {
            heap->root = push_decreased_key(heap->root, node, value);
        }

        template<typename ...Args>
        void re_emplace(Args &&... args) {
            heap->root = emplace_decreased_key(heap->root, node, std::forward<Args>(args)...);
        }
    };

    template<typename T, typename Compare>
    PairingHeap<T, Compare>::PairingHeap(const std::initializer_list<T> &list) {
        for (const auto &i : list) {
            push(i);
        }
    }

    template<typename T, typename Compare>
    PairingHeap<T, Compare>::PairingHeap(PairingHeap &&that) noexcept {
        this->root = that.root;
        this->_size = that._size;
        that.root = nullptr;
        that._size = 0;
    }

    template<typename T, typename Compare>
    PairingHeap<T, Compare> &PairingHeap<T, Compare>::operator=(PairingHeap &&that) noexcept {
        this->~PairingHeap();
        this->root = that.root;
        this->_size = that._size;
        that.root = nullptr;
        that._size = 0;
        return *this;
    }

    template<typename T, typename Compare>
    PairingHeap<T, Compare>::PairingHeap(const PairingHeap &that) {
        this->root = copy(that.root);
        this->_size = that._size;
    }

    template<typename T, typename Compare>
    PairingHeap<T, Compare> &PairingHeap<T, Compare>::operator=(const PairingHeap &that) {
        this->~PairingHeap();
        this->root = copy(that.root);
        this->_size = that._size;
        return *this;
    }

    template<typename T, typename Compare>
    PairingHeap<T, Compare>::~PairingHeap() {
        destroy(root);
    }

    template<typename T, typename Compare>
    const T &PairingHeap<T, Compare>::top() {
        return top(root);
    }

    template<typename T, typename Compare>
    void PairingHeap<T, Compare>::push(const T &t) {
        root = push(root, t);
        _size += 1;
    }

    template<typename T, typename Compare>
    template<typename... Args>
    void PairingHeap<T, Compare>::emplace(Args &&... args) noexcept {
        root = emplace(root, std::forward<Args>(args)...);
        _size += 1;
    }

    template<typename T, typename Compare>
    void PairingHeap<T, Compare>::merge(PairingHeap &that) {
        root = merge(root, that.root);
        that.root = nullptr;
        _size += that._size;
        that._size = 0;
    }

    template<typename T, typename Compare>
    size_t PairingHeap<T, Compare>::size() const {
        return _size;
    }

    template<typename T, typename Compare>
    void PairingHeap<T, Compare>::pop() {
        root = pop(root);
        _size -= 1;
    }

    template<typename T, typename Compare>
    bool PairingHeap<T, Compare>::is_empty() { return root == nullptr; }

    template<typename T, typename Compare>
    typename PairingHeap<T, Compare>::value_holder PairingHeap<T, Compare>::push_and_hold(const T &t) {
        auto m = new Node(t);
        root = merge(root, m);
        _size += 1;
        return {m, this};
    }

    template<typename T, typename Compare>
    template<typename... Args>
    typename PairingHeap<T, Compare>::value_holder PairingHeap<T, Compare>::emplace_and_hold(Args &&... args) {
        auto m = new Node(std::forward<Args>(args)...);
        root = merge(root, m);
        _size += 1;
        return {m, this};
    }

    template<typename T, typename Compare>
    bool PairingHeap<T, Compare>::empty() {
        return !root;
    }
}
#endif //DATA_STRUCTURE_FOR_LOVE_PAIRING_HEAP_HPP
