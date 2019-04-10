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

        Node *link(Node *a, Node *b) noexcept {
            if (compare(b->value, a->value)) {
                std::swap(a, b);
            }
            b->sibling = a->child;
            a->child = b;
            b->father = a;
            a->rank++;
            return a;
        }

        std::pair<Node *, Node *> extract_first(Node *heap) noexcept {
            Node *top = nullptr;
            if (heap) {
                top = heap;
                heap = heap->sibling;
                top->sibling = nullptr;
            }
            return {top, heap};
        }

        Node *insert_tree(Node *heap, Node *tree) noexcept {
            while (heap and heap->rank == tree->rank) {
                auto res = extract_first(heap);
                heap = res.second;
                auto t1 = res.first;
                tree = link(tree, t1);
            }
            tree->sibling = heap;
            return tree;
        }

        inline auto insert(Node *&heap, const T &t) noexcept {
            auto temp = new Node(t);
            return insert_tree(heap, temp);
        }

        template<typename ...Args>
        inline auto emplace(Node *&heap, Args &&... args) {
            auto temp = new Node(std::forward<Args>(args)...);
            return insert_tree(heap, temp);
        }

        std::tuple<Node *, Node *, Node *> append_tree(Node *head, Node *prev, Node *tail, Node *x) noexcept {
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

        std::tuple<Node *, Node *, Node *> append_trees(Node *h, Node *p, Node *t, Node *xs) noexcept {
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

        Node *merge(Node *h1, Node *h2) noexcept {
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

        Node *reverse(Node *h) {
            Node *prev = nullptr;
            while (h) {
                auto x = h;
                h = h->sibling;
                x->sibling = prev;
                prev = x;
            }
            return prev;
        }

        std::pair<Node *, Node *> remove_min_tree(Node *h) noexcept {
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

        const T &top(Node *h) noexcept {
            Node *min_t = nullptr;
            while (h) {
                if (!min_t || compare(h->value, min_t->value)) min_t = h;
                h = h->sibling;
            }
            return min_t->value;
        }

        std::pair<T, Node *> pop(Node *h) noexcept {
            auto temp = remove_min_tree(h);
            h = merge(temp.second, reverse(temp.first->child));
            temp.first->child = nullptr;
            std::pair<T, Node *> res = {temp.first->value, h};
            delete (temp.first);
            return res;
        }

        void destroy(Node *h) {
            if (!h) return;
            else {
                destroy(h->sibling);
                destroy(h->child);
                delete (h);
            }
        }

        Node *copy(Node *h, Node *father = nullptr) {
            if (!h)
                return nullptr;
            auto m = new Node(h->value);
            m->rank = h->rank;
            m->father = father;
            m->sibling = copy(h->sibling, father);
            m->child = copy(h->child, h);
            return m;
        }

    public:
        void push(const T &t) override {
            root = insert(root, t);
            _size++;
        }

        const T &top() override {
            return top(root);
        };

        size_t size() const override {
            return _size;
        };

        void pop() override {
            root = pop(root).second;
            _size--;
        };

        template<typename ...Args>
        void emplace(Args &&... args) {
            root = emplace(root, std::forward<Args>(args)...);
            _size++;
        }

        void merge(BinomialHeap &that) {
            root = merge(root, that.root);
            that.root = nullptr;
            _size += that.size();
        };

        BinomialHeap() = default;

        BinomialHeap(const BinomialHeap &that) {
            root = copy(that.root);
            _size = that.size();
        }

        BinomialHeap &operator=(const BinomialHeap &that) {
            destroy(root);
            root = copy(that.root);
            _size = that.size();
            return *this;
        }

        BinomialHeap(BinomialHeap &&that) noexcept {
            root = that.root;
            that.root = nullptr;
            _size = that.size();
            that._size = 0;
        }

        BinomialHeap &operator=(BinomialHeap &&that) noexcept {
            destroy(root);
            root = that.root;
            that.root = nullptr;
            _size = that.size();
            that._size = 0;
            return *this;
        }

        BinomialHeap(const std::initializer_list<T> &list) {
            for (const auto &i: list) {
                this->push(i);
            }
        }

        ~BinomialHeap() {
            destroy(root);
        }
    };
}
#endif //DATA_STRUCTURE_FOR_LOVE_BINOMIAL_HEAP_HPP
