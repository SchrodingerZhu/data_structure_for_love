//
// Created by schrodinger on 19-5-17.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_AVL_TREE_HPP
#define DATA_STRUCTURE_FOR_LOVE_AVL_TREE_HPP

#include <binary_tree_base.hpp>

namespace data_structure {
    template<class T>
    struct AVLNode : public BSTNode<T> {
        int delta;

        template<typename ...Args>
        explicit AVLNode(Args &&...args) : BSTNode<T>(std::forward<Args>(args)...), delta(0) {}
    };

    template<class T, class Node = AVLNode<T>,
            class Compare = DefaultCompare<T>, class Factory = utils::TrivialFactory<Node>>
    class AVLTree : public BSTree<T, Node, Compare, Factory> {
    protected:
        using BinTree<Node, Factory>::factory;
        using BSTree<T, Node, Compare, Factory>::compare;
        using BSTree<T, Node, Compare, Factory>::n;

        Node *left_rotate(Node *t, Node *x);

        Node *right_rotate(Node *t, Node *y);

        Node *insert_fix(Node *t, Node *parent, Node *x);

        Node *insert(Node *t, const T &key);

        Node *delete_fix(Node *t, Node *parent, Node *x);

        Node *del(Node *t, Node *x);

        bool is_leaf(Node *x) {
            return x && x->children[LEFT] == nullptr && x->children[RIGHT] == nullptr;
        }

    public:

        bool insert(const T &x) override;

        bool erase(const T &x) override;

    };

    template<class T, class Node, class Compare, class Factory>
    Node *AVLTree<T, Node, Compare, Factory>::left_rotate(Node *t, Node *x) {
        Node *parent = static_cast<Node *>(x->parent);
        Node *y = static_cast<Node *>(x->children[RIGHT]);
        Node *a = static_cast<Node *>(x->children[LEFT]);
        Node *b = static_cast<Node *>(y->children[LEFT]);
        Node *c = static_cast<Node *>(y->children[RIGHT]);
        x->replace_with(y);
        x->set_children(a, b);
        y->set_children(x, c);
        if (!parent) t = y;
        x->update();
        y->update();
        return t;
    }

    template<class T, class Node, class Compare, class Factory>
    Node *AVLTree<T, Node, Compare, Factory>::right_rotate(Node *t, Node *y) {
        Node *parent = static_cast<Node *>(y->parent);
        Node *x = static_cast<Node *>(y->children[LEFT]);
        Node *a = static_cast<Node *>(x->children[LEFT]);
        Node *b = static_cast<Node *>(x->children[RIGHT]);
        Node *c = static_cast<Node *>(y->children[RIGHT]);
        y->replace_with(x);
        y->set_children(b, c);
        x->set_children(a, y);
        if (!parent) t = x;
        y->update();
        x->update();
        return t;
    }

    template<class T, class Node, class Compare, class Factory>
    Node *AVLTree<T, Node, Compare, Factory>::insert_fix(Node *t, Node *parent, Node *x) {
        int d1, d2, dy;
        Node *p, *l, *r;
        while (parent) {
            d2 = d1 = parent->delta;
            d2 += (x == parent->children[LEFT] ? -1 : 1);
            parent->delta = d2;
            p = parent;
            l = static_cast<Node *>(parent->children[LEFT] );
            r = static_cast<Node *>(parent->children[RIGHT]);
            if (abs(d1) == 1 && abs(d2) == 0) {
                return t;
            } else if (abs(d1) == 0 && abs(d2) == 1) {
                x = parent;
                parent = static_cast<Node *>(x->parent);
            } else {
                if (d2 == 2) {
                    if (r->delta == 1) { // right-right case
                        p->delta = 0;
                        r->delta = 0;
                        t = left_rotate(t, p);
                    } else if (r->delta == -1) { // right-left case
                        dy = static_cast<Node *>(r->children[LEFT])->delta;
                        p->delta = dy == 1 ? -1 : 0;
                        static_cast<Node *>(r->children[LEFT])->delta = 0;
                        r->delta = dy == -1 ? 1 : 0;
                        t = right_rotate(t, r);
                        t = left_rotate(t, p);
                    }
                } else if (d2 == -2) {
                    if (l->delta == -1) { // left-left case
                        p->delta = 0;
                        l->delta = 0;
                        t = right_rotate(t, p);
                    } else if (l->delta == 1) { // left-right case
                        dy = static_cast<Node *>(l->children[RIGHT])->delta;
                        l->delta = dy == 1 ? -1 : 0;
                        static_cast<Node *>(l->children[RIGHT])->delta = 0;
                        p->delta = dy == -1 ? 1 : 0;
                        t = left_rotate(t, l);
                        t = right_rotate(t, p);
                    }
                }
                break;
            }
        }
        return t;
    }

    template<class T, class Node, class Compare, class Factory>
    Node *AVLTree<T, Node, Compare, Factory>::insert(Node *t, const T &key) {
        Node *root = t;
        Node *parent = nullptr;
        while (t) {
            parent = t;
            auto res = compare(key, t->x);
            if (res == Less) t = static_cast<Node *>(t->children[LEFT]);
            else if (res == Greater) t = static_cast<Node *>(t->children[RIGHT]);
            else return nullptr;
        }


        Node *x = factory.construct(key);
        if (!parent) {
            root = x;
        } else if (compare(key, parent->x) == Less) {
            parent->set_left(x);
        } else {
            parent->set_right(x);
        }
        n++;
        this->bottom_up_update(x);
        return insert_fix(root, parent, x);
    }

    template<class T, class Node, class Compare, class Factory>
    Node *AVLTree<T, Node, Compare, Factory>::delete_fix(Node *t, Node *parent, Node *x) {
        int d1, d2, dy;
        Node *p, *l, *r;
        while (parent) {
            d2 = d1 = parent->delta;
            d2 += (x == parent->children[LEFT] ? 1 : -1);
            if (is_leaf(parent)) d2 = 0;
            parent->delta = d2;
            p = parent;
            l = static_cast<Node *>(parent->children[LEFT]);
            r = static_cast<Node *>(parent->children[RIGHT]);
            if (abs(d1) == 1 && abs(d2) == 0) {
                x = parent;
                parent = static_cast<Node *>(x->parent);
            } else if (abs(d1) == 0 && abs(d2) == 1) {
                return t;
            } else {
                if (d2 == 2) {
                    if (r->delta == 1) {  // right-right case
                        p->delta = r->delta = 0;
                        parent = r;
                        t = left_rotate(t, p);
                    } else if (r->delta == -1) { // right-left case
                        dy = static_cast<Node *>(r->children[LEFT])->delta;
                        p->delta = dy == 1 ? -1 : 0;
                        static_cast<Node *>(r->children[LEFT])->delta = 0;
                        r->delta = dy == -1 ? 1 : 0;
                        parent = static_cast<Node *>(r->children[LEFT]);
                        t = right_rotate(t, r);
                        t = left_rotate(t, p);
                    } else { // delete specific right-right case
                        p->delta = 1;
                        r->delta--;
                        t = left_rotate(t, p);
                        break; // no further height change
                    }
                } else if (d2 == -2) {
                    if (l->delta == -1) { // left-left case
                        p->delta = l->delta = 0;
                        parent = l;
                        t = right_rotate(t, p);
                    } else if (l->delta == 1) { // left-right case
                        dy = static_cast<Node *>(l->children[RIGHT])->delta;
                        l->delta = dy == 1 ? -1 : 0;
                        static_cast<Node *>(l->children[RIGHT])->delta = 0;
                        p->delta = dy == -1 ? 1 : 0;
                        parent = static_cast<Node *>(l->children[RIGHT]);
                        t = left_rotate(t, l);
                        t = right_rotate(t, p);
                    } else { // delete specific left-left case
                        p->delta = -1;
                        l->delta++;
                        t = right_rotate(t, p);
                        break; // no further height change
                    }
                }
                // the 4 rebalance cases cause height decrease, go on bottom-up update
                x = parent;
                parent = static_cast<Node *>(x->parent);
            }
        }
        if (!parent) return x; // delete the root
        return t;
    }

    template<class T, class Node, class Compare, class Factory>
    Node *AVLTree<T, Node, Compare, Factory>::del(Node *t, Node *x) {
        if (!x) return t;
        Node *y, *parent = static_cast<Node *>(x->parent);
        if (!x->children[LEFT]) {
            y = static_cast<Node *>(x->replace_with(static_cast<Node *>(x->children[RIGHT])));
        } else if (!x->children[RIGHT]) {
            y = static_cast<Node *>(x->replace_with(static_cast<Node *>(x->children[LEFT])));
        } else {
            y = static_cast<Node *>(this->min_node(static_cast<Node *>(x->children[RIGHT])));
            x->x = y->x;
            parent = static_cast<Node *>(y->parent);
            x = y;
            y = static_cast<Node *>(y->replace_with(static_cast<Node *>(y->children[RIGHT])));
        }
        t = delete_fix(t, parent, y);
        factory.destroy(x);
        return t;
    }

    template<class T, class Node, class Compare, class Factory>
    bool AVLTree<T, Node, Compare, Factory>::insert(const T &x) {
        auto new_root = insert(this->root, x);
        if (!new_root) return false;
        else {
            return this->root = new_root;
        }
    }

    template<class T, class Node, class Compare, class Factory>
    bool AVLTree<T, Node, Compare, Factory>::erase(const T &x) {
        auto node = this->find_last(x);
        if (node && compare(node->x, x) == Eq) {
            n--;
            this->root = del(this->root, node);
            return true;
        }
        return false;
    }

}
#endif //DATA_STRUCTURE_FOR_LOVE_AVL_TREE_HPP
