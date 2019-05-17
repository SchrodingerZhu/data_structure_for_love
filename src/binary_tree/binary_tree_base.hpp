//
// Created by schrodinger on 3/4/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BINARY_TREE_BASE_HPP
#define DATA_STRUCTURE_FOR_LOVE_BINARY_TREE_BASE_HPP

#include <node_factory.hpp>
#include <utility>
#include <stack>
#include <iostream>
// min
// max
// merge
//
namespace data_structure {
    enum Direction {
        LEFT = 0, RIGHT = 1
    };


    struct Node {
        Node *parent = nullptr, *children[2] = {nullptr, nullptr};

        virtual ~Node() = default;


        inline void set_left(Node *n) noexcept {
            children[LEFT] = n;
            if (n) n->parent = this;
        }

        inline void set_right(Node *n) noexcept {
            children[RIGHT] = n;
            if (n) n->parent = this;
        }

        inline void set_children(Node *n1, Node *n2) noexcept {
            set_left(n1);
            set_right(n2);
        }

        inline Node *replace_with(Node *y) noexcept {
            return replace(parent, this, y);
        }

        inline Node *sibling() noexcept {
            return parent->children[parent->children[LEFT] == this];
        }

        inline Node *uncle() noexcept {
            return parent->sibling();
        }

        inline Node *grandparent() noexcept {
            return parent->parent;
        }


        inline static Node *replace(Node *parent, Node *x, Node *y) noexcept {
            if (parent == nullptr) {
                if (y) y->parent = nullptr;
            } else if (parent->children[LEFT] == x) {
                parent->set_left(y);
            } else {
                parent->set_right(y);
            }
            if (x) x->parent = nullptr;
            return y;
        }

    };


    template<class TreeNode, class Factory>
    class BinTree {
    protected:
        TreeNode *root = nullptr;
        Factory factory{};

        virtual void clear() {
            Node *u = root, *prev = nullptr, *next;
            while (u != nullptr) {
                if (prev == u->parent) {
                    if (u->children[LEFT] != nullptr) next = u->children[LEFT];
                    else if (u->children[RIGHT] != nullptr) next = u->children[RIGHT];
                    else next = u->parent;
                } else if (prev == u->children[LEFT]) {
                    if (u->children[RIGHT] != nullptr) next = u->children[RIGHT];
                    else next = u->parent;
                } else {
                    next = u->parent;
                }
                prev = u;
                if (next == u->parent)
                    factory.destroy(static_cast<TreeNode *>(u));
                u = next;
            }
            root = nullptr;
        }
    public:
        virtual ~BinTree() {
            clear();
        }
    };

    template<class T>
    struct BSTNode : public Node {
        T x;

        template<typename ...Args>
        explicit BSTNode(Args ...args) : x(std::forward<Args>(args)...) {}

        virtual void update() {}

    };

    template<class T>
    struct WeightedBSTNode : public Node {
        T x;
        size_t weight = 0;

        template<typename ...Args>
        explicit WeightedBSTNode(Args &&...args) : x(std::forward<Args>(args)...) {}

        virtual void update() {
            weight = 1 + (this->children[LEFT] ? static_cast<WeightedBSTNode *>(this->children[LEFT])->weight : 0) +
                     (this->children[RIGHT] ? static_cast<WeightedBSTNode *>(this->children[RIGHT])->weight : 0);
        }
    };


    enum Relation {
        Eq, Less, Greater
    };

    template<class T>
    struct DefaultCompare {
        constexpr Relation operator()(const T &a, const T &b) const noexcept {
            if (a == b) return Eq;
            else if (a < b) return Less;
            else return Greater;
        }
    };

    template<class T, class Node = BSTNode<T>,
            class Compare = DefaultCompare<T>, class Factory = utils::TrivialFactory<Node>>
    class BSTree : public BinTree<Node, Factory> {
    protected:
        constexpr static Compare compare{};
        size_t n = 0;

        virtual Node *find_last(const T &x);

        virtual bool adopt(Node *p, Node *u);

        virtual void splice(Node *u);

        virtual void erase(Node *u);

        virtual void rotate(Node *u, Direction direction);

        virtual bool insert(Node *u);

        virtual void bottom_up_update(Node *u);

        static Node *min_node(Node *u);

        static Node *max_node(Node *u);

        static Node *pred_node(Node *u);

        static Node *succ_node(Node *u);

    public:
        class walker;

        class iterator;

        virtual bool insert(const T &x);

        virtual bool erase(const T &x);

        virtual walker find(const T &x);

        virtual size_t size();

        virtual void clear();

        virtual walker min();

        virtual walker max();

        virtual walker top();

        virtual walker pred(const T &x);

        virtual walker succ(const T &x);

        virtual bool contains(const T &x);

        iterator begin();

        iterator end();

    };

    template<class T, class Node, class Compare, class Factory>
    class BSTree<T, Node, Compare, Factory>::walker {
        Node *node;
    public:
        explicit walker(Node *node) : node(node) {}

        Node *unsafe_cast() { return node; }

        T &get() { return node->x; }

        const T &get() const { return node->x; }

        T &operator*() { return get(); }

        const T &operator*() const { return get(); }

        T *operator->() { return &get(); }

        const T *operator->() const { return &get(); }

        void go_up() { node = static_cast<Node *>(node->parent); }

        void go_left() { node = static_cast<Node *>(node->children[LEFT]); }

        void go_right() { node = static_cast<Node *>(node->children[RIGHT]); }

        void go_min() { node = min_node(node); }

        void go_max() { node = max_node(node); }

        void go_succ() { node = succ_node(node); }

        void go_pred() { node = succ_node(node); }

        bool valid() { return node; }


    };

    template<class T, class Node, class Compare, class Factory>
    class BSTree<T, Node, Compare, Factory>::iterator {
        Node *node;
        BSTree *tree;
    public:
        explicit iterator(Node *node, BSTree *tree) : node(node), tree(tree) {}

        bool operator==(const iterator &that) { return node == that.node; }

        bool operator!=(const iterator &that) { return node != that.node; }

        T &operator*() { return node->x; }

        const T &operator*() const { return node->x; }

        T *operator->() { return &node->x; }

        const T *operator->() const { return &node->x; }

        iterator &operator++() {
            node = succ_node(node);
            return *this;
        }

        const iterator operator++(int) {
            auto m = *this;
            node = succ_node(node);
            return m;
        }

        iterator &operator--() {
            if (!node) node = tree->max().unsafe_cast();
            else node = pred_node(node);
            return *this;
        }

        const iterator operator--(int) {
            auto m = *this;
            if (!node) node = tree->max().unsafe_cast();
            else node = pred_node(node);
            return m;
        }
    };

    template<class T, class Node, class Compare, class Factory>
    Node *BSTree<T, Node, Compare, Factory>::find_last(const T &x) {
        Node *w = this->root, *prev = nullptr;
        while (w != nullptr) {
            prev = w;
            switch (compare(x, w->x)) {
                case Less:
                    w = static_cast<Node *>(w->children[LEFT]);
                    break;
                case Greater:
                    w = static_cast<Node *>(w->children[RIGHT]);
                    break;
                case Eq:
                    return w;

            }
        }
        return prev;
    }

    template<class T, class Node, class Compare, class Factory>
    bool BSTree<T, Node, Compare, Factory>::adopt(Node *p, Node *u) {
        if (p == nullptr) {
            this->root = u;
        } else {
            switch (compare(u->x, p->x)) {
                case Less:
                    p->children[LEFT] = u;
                    break;
                case Greater:
                    p->children[RIGHT] = u;
                    break;
                case Eq:
                    return false;

            }
            u->parent = p;
        }
        n++;
        bottom_up_update(u);
        return true;
    }

    template<class T, class Node, class Compare, class Factory>
    void BSTree<T, Node, Compare, Factory>::splice(Node *u) {
        Node *s, *p;
        if (u->children[LEFT] != nullptr) {
            s = static_cast<Node *>(u->children[LEFT]);
        } else {
            s = static_cast<Node *>(u->children[RIGHT]);
        }
        if (u == this->root) {
            this->root = s;
            p = nullptr;
        } else {
            p = static_cast<Node *>(u->parent);
            if (p->children[LEFT] == u) {
                p->children[LEFT] = s;
            } else {
                p->children[RIGHT] = s;
            }
        }
        if (s != nullptr) {
            s->parent = p;
        }
        bottom_up_update(p);
        n--;
    }

    template<class T, class Node, class Compare, class Factory>
    void BSTree<T, Node, Compare, Factory>::erase(Node *u) {
        if (u->children[LEFT] == nullptr || u->children[RIGHT] == nullptr) {
            splice(u);
            this->factory.destroy(u);
        } else {
            Node *w = static_cast<Node *>(u->children[RIGHT]);
            while (w->children[LEFT] != nullptr)
                w = static_cast<Node *>(w->children[LEFT]);
            u->x = w->x;
            splice(w);
            this->factory.destroy(w);
        }
    }

    template<class T, class Node, class Compare, class Factory>
    void BSTree<T, Node, Compare, Factory>::rotate(Node *u, Direction direction) {
        Node *w = static_cast<Node *>(u->children[!direction]);
        w->parent = u->parent;
        if (w->parent) {
            if (w->parent->children[LEFT] == u) w->parent->children[LEFT] = w;
            else w->parent->children[RIGHT] = w;
        }
        u->children[!direction] = w->children[direction];
        if (u->children[!direction]) {
            u->children[!direction]->parent = u;
        }
        u->parent = w;
        w->children[direction] = u;
        if (u == this->root) {
            this->root = w;
            this->root->parent = nullptr;
        }
        u->update();
        w->update();
    }

    template<class T, class Node, class Compare, class Factory>
    bool BSTree<T, Node, Compare, Factory>::insert(Node *u) {
        return adopt(find_last(u->x), u);
    }

    template<class T, class Node, class Compare, class Factory>
    void BSTree<T, Node, Compare, Factory>::bottom_up_update(Node *u) {
        while (u) {
            u->update();
            u = static_cast<Node *>(u->parent);
        }
    }

    template<class T, class Node, class Compare, class Factory>
    bool BSTree<T, Node, Compare, Factory>::insert(const T &x) {
        auto p = find_last(x);
        if (!p || compare(x, p->x) != Eq) {
            adopt(p, this->factory.construct(x));
            return true;
        }
        return false;
    }

    template<class T, class Node, class Compare, class Factory>
    bool BSTree<T, Node, Compare, Factory>::erase(const T &x) {
        Node *u = find_last(x);
        if (u && compare(x, u->x) == Eq) {
            erase(u);
            return true;
        }
        return false;
    }

    template<class T, class Node, class Compare, class Factory>
    Node *BSTree<T, Node, Compare, Factory>::min_node(Node *u) {
        while (u && u->children[LEFT]) {
            u = static_cast<Node *>(u->children[LEFT]);
        }
        return u;
    }

    template<class T, class Node, class Compare, class Factory>
    Node *BSTree<T, Node, Compare, Factory>::max_node(Node *u) {
        while (u && u->children[RIGHT]) {
            u = static_cast<Node *>(u->children[RIGHT]);
        }
        return u;
    }

    template<class T, class Node, class Compare, class Factory>
    Node *BSTree<T, Node, Compare, Factory>::pred_node(Node *u) {
        if (!u) return nullptr;
        if (u->children[LEFT]) return max_node(static_cast<Node *>(u->children[LEFT]));
        else {
            while (u->parent && u != u->parent->children[RIGHT]) {
                u = static_cast<Node *>(u->parent);
            }
            return static_cast<Node *>(u->parent);
        }
    }

    template<class T, class Node, class Compare, class Factory>
    Node *BSTree<T, Node, Compare, Factory>::succ_node(Node *u) {
        if (!u) return nullptr;
        if (u->children[RIGHT]) return min_node(static_cast<Node *>(u->children[RIGHT]));
        else {
            while (u->parent && u != u->parent->children[LEFT]) {
                u = static_cast<Node *>(u->parent);
            }
            return static_cast<Node *>(u->parent);
        }
    }

    template<class T, class Node, class Compare, class Factory>
    typename BSTree<T, Node, Compare, Factory>::walker BSTree<T, Node, Compare, Factory>::top() {
        return BSTree::walker(this->root);
    }

    template<class T, class Node, class Compare, class Factory>
    typename BSTree<T, Node, Compare, Factory>::walker BSTree<T, Node, Compare, Factory>::find(const T &x) {
        Node *w = static_cast<Node *>(this->root);
        while (w != nullptr) {
            auto comp = compare(x, w->x);
            if (comp == Less) {
                w = static_cast<Node *>(w->children[LEFT]);
            } else if (comp == Greater) {
                w = static_cast<Node *>(w->children[RIGHT]);
            } else {
                return walker(w);
            }
        }
        return walker(nullptr);
    }

    template<class T, class Node, class Compare, class Factory>
    size_t BSTree<T, Node, Compare, Factory>::size() {
        return n;
    }

    template<class T, class Node, class Compare, class Factory>
    void BSTree<T, Node, Compare, Factory>::clear() {
        BinTree<Node, Factory>::clear();
        n = 0;
    }

    template<class T, class Node, class Compare, class Factory>
    typename BSTree<T, Node, Compare, Factory>::walker BSTree<T, Node, Compare, Factory>::min() {
        return BSTree::walker(min_node(this->root));
    }

    template<class T, class Node, class Compare, class Factory>
    typename BSTree<T, Node, Compare, Factory>::walker BSTree<T, Node, Compare, Factory>::max() {
        return BSTree::walker(max_node(this->root));
    }

    template<class T, class Node, class Compare, class Factory>
    typename BSTree<T, Node, Compare, Factory>::walker BSTree<T, Node, Compare, Factory>::succ(const T &x) {
        Node *u = static_cast<Node *>(this->root);
        while (u) {
            if (compare(u->x, x) != Greater) u = static_cast<Node *>(u->children[RIGHT]);
            else {
                auto p = pred_node(u);
                if (p && compare(p->x, x) == Greater) u = p;
                else break;
            }
        }
        return walker(u);
    }

    template<class T, class Node, class Compare, class Factory>
    typename BSTree<T, Node, Compare, Factory>::walker BSTree<T, Node, Compare, Factory>::pred(const T &x) {
        Node *u = static_cast<Node *>(this->root);
        while (u) {
            if (compare(u->x, x) != Less) u = static_cast<Node *>(u->children[LEFT]);
            else {
                auto p = succ_node(u);
                if (p && compare(p->x, x) == Less) u = p;
                else break;
            }
        }
        return walker(u);
    }

    template<class T, class Node, class Compare, class Factory>
    bool BSTree<T, Node, Compare, Factory>::contains(const T &x) {
        auto t = find_last(x);
        return compare(t->x, x) == Eq;
    }

    template<class T, class Node, class Compare, class Factory>
    typename BSTree<T, Node, Compare, Factory>::iterator BSTree<T, Node, Compare, Factory>::begin() {
        return BSTree::iterator(min_node(this->root), this);
    }

    template<class T, class Node, class Compare, class Factory>
    typename BSTree<T, Node, Compare, Factory>::iterator BSTree<T, Node, Compare, Factory>::end() {
        return BSTree::iterator(nullptr, this);
    }


}
#endif //DATA_STRUCTURE_FOR_LOVE_BINARY_TREE_BASE_HPP
