//
// Created by schrodinger on 19-5-16.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_TREAP_HPP
#define DATA_STRUCTURE_FOR_LOVE_TREAP_HPP

#include <binary_tree_base.hpp>
#include <static_random_helper.hpp>
namespace data_structure {
    template<class T>
    struct TreapNode : public BSTNode<T> {
        std::size_t p;

        template<typename ...Args>
        explicit TreapNode(std::size_t p, Args &&...args) : p(p), BSTNode<T>(std::forward<Args>(args)...) {}
    };

    template<class T, class Node = TreapNode<T>,
            class Compare = utils::DefaultCompare<T>, class Factory = utils::PoolFactory<Node>>
    class Treap : public BSTree<T, Node, Compare, Factory> {
    protected:
        using BinTree<Node, Factory>::root;
        using BinTree<Node, Factory>::factory;
        using BSTree<T, Node, Compare, Factory>::compare;
        using BSTree<T, Node, Compare, Factory>::n;
        static utils::RandomIntGen<std::size_t> rand;

        void bubble_up(Node *u);

        void trickle_down(Node *u);

    public:
        virtual bool insert(const T &x);

        virtual bool erase(const T &x);
    };

    template<class T, class Node, class Compare, class Factory>
    utils::RandomIntGen<std::size_t> Treap<T, Node, Compare, Factory>::rand{};

    template<class T, class Node, class Compare, class Factory>
    void Treap<T, Node, Compare, Factory>::bubble_up(Node *u) {
        while (u->parent && static_cast<Node *>(u->parent)->p > u->p) {
            if (u->parent->children[LEFT] == u) {
                this->rotate(static_cast<Node *>(u->parent), RIGHT);
            } else {
                this->rotate(static_cast<Node *>(u->parent), LEFT);
            }
        }
        if (!u->parent) { root = u; }
    }

    template<class T, class Node, class Compare, class Factory>
    void Treap<T, Node, Compare, Factory>::trickle_down(Node *u) {
        while (u->children[LEFT] || u->children[RIGHT]) {
            if (!u->children[LEFT]) {
                this->rotate(u, LEFT);
            } else if (!u->children[RIGHT]) {
                this->rotate(u, RIGHT);
            } else if (static_cast<Node *>(u->children[LEFT])->p < static_cast<Node *>(u->children[RIGHT])->p) {
                this->rotate(u, RIGHT);
            } else {
                this->rotate(u, LEFT);
            }
            if (root == u) {
                root = static_cast<Node *>(u->parent);
            }
        }

    }

    template<class T, class Node, class Compare, class Factory>
    bool Treap<T, Node, Compare, Factory>::insert(const T &x) {
        auto p = this->find_last(x);
        if (!p || compare(x, p->x) != utils::Eq) {
            auto node = this->factory.construct(rand(), x);
            this->adopt(p, node);
            bubble_up(node);
            return true;
        }
        return false;
    }

    template<class T, class Node, class Compare, class Factory>
    bool Treap<T, Node, Compare, Factory>::erase(const T &x) {
        auto p = this->find_last(x);
        if (p && compare(x, p->x) == utils::Eq) {
            trickle_down(p);
            this->splice(p);
            factory.destroy(p);
            return true;
        }
        return false;
    }
}
#endif //DATA_STRUCTURE_FOR_LOVE_TREAP_HPP
