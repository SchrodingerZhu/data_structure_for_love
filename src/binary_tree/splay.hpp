//
// Created by schrodinger on 19-5-16.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_SPLAY_HPP
#define DATA_STRUCTURE_FOR_LOVE_SPLAY_HPP

#include <binary_tree_base.hpp>

namespace data_structure {
    template<class T, class Node = BSTNode<T>,
            class Compare = utils::DefaultCompare<T>, class Factory = utils::PoolFactory<Node>>
    class Splay : public BSTree<T, Node, Compare, Factory> {
    protected:
        using BinTree<Node, Factory>::root;
        using BinTree<Node, Factory>::factory;
        using BSTree<T, Node, Compare, Factory>::compare;
        using BSTree<T, Node, Compare, Factory>::n;

        void splay(Node *u, Node **target);

        virtual Node *find_last(const T &x);


    public:
        virtual bool insert(const T &x);

        virtual bool erase(const T &x);


    };

    template<class T, class Node, class Compare, class Factory>
    void Splay<T, Node, Compare, Factory>::splay(Node *u, Node **target) {
        while ((*target) != u) {
            if (u->parent == (*target)) {
                this->rotate((*target), (*target)->children[LEFT] == u ? RIGHT : LEFT);
            } else if (u == u->parent->children[LEFT] && u->parent == u->parent->parent->children[LEFT]) {
                this->rotate(static_cast<Node *>(u->parent->parent), RIGHT);
                this->rotate(static_cast<Node *>(u->parent), RIGHT);
            } else if (u == u->parent->children[RIGHT] && u->parent == u->parent->parent->children[RIGHT]) {
                this->rotate(static_cast<Node *>(u->parent->parent), LEFT);
                this->rotate(static_cast<Node *>(u->parent), LEFT);
            } else if (u == u->parent->children[RIGHT] && u->parent == u->parent->parent->children[LEFT]) {
                this->rotate(static_cast<Node *>(u->parent), LEFT);
                this->rotate(static_cast<Node *>(u->parent), RIGHT);
            } else {
                this->rotate(static_cast<Node *>(u->parent), RIGHT);
                this->rotate(static_cast<Node *>(u->parent), LEFT);
            }
        }
    }

    template<class T, class Node, class Compare, class Factory>
    Node *Splay<T, Node, Compare, Factory>::find_last(const T &x) {
        auto m = BSTree<T, Node, Compare, Factory>::find_last(x);
        splay(m, &root);
        return root;
    }

    template<class T, class Node, class Compare, class Factory>
    bool Splay<T, Node, Compare, Factory>::insert(const T &x) {
        auto p = BSTree<T, Node, Compare, Factory>::find_last(x);
        if (!p || compare(x, p->x) != utils::Eq) {
            auto node = this->factory.construct(x);
            this->adopt(p, node);
            splay(node, &root);
            return true;
        }
        return false;
    }

    template<class T, class Node, class Compare, class Factory>
    bool Splay<T, Node, Compare, Factory>::erase(const T &x) {
        auto p = this->find_last(x);
        if (!p || compare(x, p->x) != utils::Eq) { return false; }
        if (!p->children[LEFT] || !p->children[RIGHT]) {
            this->splice(p);
            factory.destroy(p);
        } else {
            splay(this->max_node(static_cast<Node *>(p->children[LEFT])),
                  reinterpret_cast<Node **>(&p->children[LEFT]));
            p->children[RIGHT]->parent = p->children[LEFT];
            p->children[LEFT]->children[RIGHT] = p->children[RIGHT];
            root = static_cast<Node *>(p->children[LEFT]);
            p->children[LEFT]->parent = nullptr;
            root->update();
            factory.destroy(p);
            n--;
        }
        return true;
    }
}
#endif //DATA_STRUCTURE_FOR_LOVE_SPLAY_HPP
