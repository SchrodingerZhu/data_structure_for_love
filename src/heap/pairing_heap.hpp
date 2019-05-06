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

        static void destroy(Node *h) noexcept {
            Node *t = nullptr;
            if (h) {
                while ((t = h->children)) {
                    h->children = h->children->next;
                    destroy(t);
                }
            }
            delete h;
        }

        static Node *copy(Node *node, Node *parent = nullptr) {
            if (!node)
                return nullptr;
            auto p = new Node(node->value);
            p->parent = parent;
            if (node->children) p->children = copy(node->children, p);
            if (node->next) p->next = copy(node->next);
            return p;
        }

        static Node *push_front(Node *list, Node *x) noexcept {
            x->next = list;
            return x;
        }

        static Node *remove_node(Node *list, Node *x) noexcept {
            auto p = list;
            if (x == list) return x->next;
            while (p && p->next != x) p = p->next;
            if (p) p->next = x->next;
            return list;
        }

        static Node *merge(Node *h1, Node *h2) noexcept {
            if (!h1) return h2;
            if (!h2) return h1;
            if (compare(h2->value, h1->value)) std::swap(h1, h2);
            h2->next = h1->children;
            h1->children = h2;
            h2->parent = h1;
            h1->next = nullptr;
            return h1;
        }

        static Node *push(Node *h, const T &x) noexcept {
            return merge(h, new Node(x));
        }

        template<typename ...Args>
        static Node *emplace(Node *h, Args &&...args) noexcept {
            return merge(h, new Node(std::forward<Args>(args)...));
        }

        static const T &top(Node *h) noexcept {
            return h->value;
        }


        static Node *push_decreased_key(Node *h, Node *x, const T &key) noexcept {
            x->value = key;
            if (x->parent) {
                x->parent->children = remove_node(x->parent->children, x);
                x->parent = nullptr;
                return merge(h, x);
            }
            return h;
        }

        template<typename ...Args>
        static Node *emplace_decreased_key(Node *h, Node *x, Args &&...args) noexcept {
            x->value = T(std::forward<Args>(args)...);
            if (x->parent) {
                x->parent->children = remove_node(x->parent->children, x);
                x->parent = nullptr;
                return merge(h, x);
            }
            return h;
        }

        static Node *pop(Node *h) noexcept {
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

        Node *root = nullptr;
        size_t _size = 0;
    public:

        class value_holder {
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

        PairingHeap() = default;

        PairingHeap(const std::initializer_list<T> &list) {
            for (const auto &i : list) {
                push(i);
            }
        }

        PairingHeap(PairingHeap &&that) noexcept {
            this->root = that.root;
            this->_size = that._size;
            that.root = nullptr;
            that._size = 0;
        }

        PairingHeap &operator=(PairingHeap &&that) noexcept {
            this->~PairingHeap();
            this->root = that.root;
            this->_size = that._size;
            that.root = nullptr;
            that._size = 0;
            return *this;
        }

        PairingHeap(const PairingHeap &that) {
            this->root = copy(that.root);
            this->_size = that._size;
        }

        PairingHeap &operator=(const PairingHeap &that) {
            this->~PairingHeap();
            this->root = copy(that.root);
            this->_size = that._size;
            return *this;
        };


        ~PairingHeap() {
            destroy(root);
        }

        const T &top() override {
            return top(root);
        }

        void push(const T &t) override {
            root = push(root, t);
            _size += 1;
        }

        template<typename ...Args>
        void emplace(Args &&... args) noexcept {
            root = emplace(root, std::forward<Args>(args)...);
            _size += 1;
        }

        void merge(PairingHeap &that) {
            root = merge(root, that.root);
            that.root = nullptr;
            _size += that._size;
            that._size = 0;
        }

        size_t size() const override {
            return _size;
        };

        void pop() override {
            root = pop(root);
            _size -= 1;
        }

        bool is_empty() { return root == nullptr; }

        value_holder push_and_hold(const T &t) {
            auto m = new Node(t);
            root = merge(root, m);
            _size += 1;
            return {m, this};
        }

        template<typename ...Args>
        value_holder emplace_and_hold(Args &&...args) {
            auto m = new Node(std::forward<Args>(args)...);
            root = merge(root, m);
            _size += 1;
            return {m, this};
        }


    };
}
#endif //DATA_STRUCTURE_FOR_LOVE_PAIRING_HEAP_HPP
