//
// Created by schrodinger on 19-5-16.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_RE_TREE_HPP
#define DATA_STRUCTURE_FOR_LOVE_RE_TREE_HPP

#include <binary_tree_base.hpp>

namespace data_structure {
    enum class Color {
        RED, BLACK, DOUBLY_BLACK
    };

    template<class T>
    struct RBTNode : public BSTNode<T> {
        Color color;

        explicit RBTNode(const T &x) : BSTNode<T>(x), color(Color::RED) {}

        template<typename ...Args>
        explicit RBTNode(Color col, Args &&...args) : color(col), BSTNode<T>(std::forward<Args>(args)...) {}

    };


    template<class T, class Node = RBTNode<T>,
            class Compare = DefaultCompare<T>, class Factory = utils::PoolFactory<Node>>
    class RbTree : public BSTree<T, Node, Compare, Factory> {
    protected:
        using BinTree<Node, Factory>::factory;
        using BSTree<T, Node, Compare, Factory>::compare;
        using BSTree<T, Node, Compare, Factory>::n;

        template<typename _Node, typename _Color, typename ...More>
        void set_colors(_Node *a, _Color b, More ...more) {
            set_colors<_Node, _Color>(a, b);
            set_colors(more...);
        }

        template<typename _Node, typename _Color>
        void set_colors(_Node *a, _Color b) { a->color = b; }

        Node *left_rotate(Node *t, Node *x);

        Node *right_rotate(Node *t, Node *y);

        Node *insert_fix(Node *t, Node *x);

        Node *insert(Node *t, const T &key);

        void remove(Node *x);

        bool is_red(Node *x);

        bool is_black(Node *x);

        Node *blacken(Node *x);

        Node *make_black(Node *parent, Node *x);

        Node *delete_fix(Node *t, Node *db, bool is_dbe);

        Node *del(Node *t, Node *x);

    public:

        bool insert(const T &x) override;

        bool erase(const T &x) override;

    };

    template<class T, class Node, class Compare, class Factory>
    Node *RbTree<T, Node, Compare, Factory>::left_rotate(Node *t, Node *x) {
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
    Node *RbTree<T, Node, Compare, Factory>::right_rotate(Node *t, Node *y) {
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
    Node *RbTree<T, Node, Compare, Factory>::insert_fix(Node *t, Node *x) {
        while (x->parent && static_cast<Node *>(x->parent)->color == Color::RED) {
            if (x->uncle() && static_cast<Node *>(x->uncle())->color == Color::RED) {
                set_colors(static_cast<Node *>(x->parent), Color::BLACK,
                           static_cast<Node *>(x->grandparent()), Color::RED,
                           static_cast<Node *>(x->uncle()), Color::BLACK);
                x = static_cast<Node *>(x->grandparent());
            } else {
                if (x->parent == x->grandparent()->children[LEFT]) {
                    if (x == x->parent->children[RIGHT]) {
                        x = static_cast<Node *>(x->parent);
                        t = left_rotate(t, x);
                    }
                    set_colors(static_cast<Node *>(x->parent), Color::BLACK,
                               static_cast<Node *>(x->grandparent()), Color::RED);
                    t = right_rotate(t, static_cast<Node *>(x->grandparent()));
                } else {
                    if (x == static_cast<Node *>(x->parent->children[LEFT])) {
                        x = static_cast<Node *>(x->parent);
                        t = right_rotate(t, x);
                    }
                    set_colors(static_cast<Node *>(x->parent), Color::BLACK,
                               static_cast<Node *>(x->grandparent()), Color::RED);
                    t = left_rotate(t, static_cast<Node *>(x->grandparent()));
                }
            }
        }
        t->color = Color::BLACK;
        return t;
    }

    template<class T, class Node, class Compare, class Factory>
    Node *RbTree<T, Node, Compare, Factory>::insert(Node *t, const T &key) {
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
        return insert_fix(root, x);
    }

    template<class T, class Node, class Compare, class Factory>
    void RbTree<T, Node, Compare, Factory>::remove(Node *x) {
        if (x) {
            x->parent = x->children[LEFT] = x->children[RIGHT] = nullptr;
            factory.destroy(x);
        }
    }

    template<class T, class Node, class Compare, class Factory>
    bool RbTree<T, Node, Compare, Factory>::is_red(Node *x) {
        return x != nullptr && x->color == Color::RED;
    }

    template<class T, class Node, class Compare, class Factory>
    bool RbTree<T, Node, Compare, Factory>::is_black(Node *x) {
        return x == nullptr || x->color == Color::BLACK;
    }

    template<class T, class Node, class Compare, class Factory>
    Node *RbTree<T, Node, Compare, Factory>::blacken(Node *x) {
        x->color = is_red(x) ? Color::BLACK : Color::DOUBLY_BLACK;
        return x;
    }

    template<class T, class Node, class Compare, class Factory>
    Node *RbTree<T, Node, Compare, Factory>::make_black(Node *parent, Node *x) {
        if (!parent && !x)
            return nullptr;
        if (!x)
            return static_cast<Node *>(Node::replace(parent, x, factory.construct(Color::DOUBLY_BLACK, parent->x)));
        return blacken(x);
    }

    template<class T, class Node, class Compare, class Factory>
    Node *RbTree<T, Node, Compare, Factory>::delete_fix(Node *t, Node *db, bool is_dbe) {
        Node *deb = is_dbe ? db : nullptr;
        if (!db) return nullptr;
        while (db != t && db->color == Color::DOUBLY_BLACK) {
            if (db->sibling() != nullptr) {
                if (is_red(static_cast<Node *>(db->sibling()))) {
                    set_colors(static_cast<Node *>(db->parent), Color::RED,
                               static_cast<Node *>(db->sibling()), Color::BLACK);
                    if (db == static_cast<Node *>(static_cast<Node *>(db->parent)->children[LEFT]))
                        t = left_rotate(t, static_cast<Node *>(db->parent));
                    else
                        t = right_rotate(t, static_cast<Node *>(db->parent));
                } else if (is_black(static_cast<Node *>(db->sibling())) &&
                           is_red(static_cast<Node *>(db->sibling()->children[LEFT]))) {
                    if (db == static_cast<Node *>(static_cast<Node *>(db->parent)->children[LEFT])) {
                        set_colors(db, Color::BLACK,
                                   static_cast<Node *>(db->parent), Color::BLACK,
                                   static_cast<Node *>(db->sibling()->children[LEFT]),
                                   static_cast<Node *>(db->parent)->color);
                        t = right_rotate(t, static_cast<Node *>(db->sibling()));
                        t = left_rotate(t, static_cast<Node *>(db->parent));
                    } else {
                        set_colors(db, Color::BLACK,
                                   static_cast<Node *>(db->parent), Color::BLACK,
                                   static_cast<Node *>(db->sibling()), static_cast<Node *>(db->parent)->color,
                                   static_cast<Node *>(db->sibling()->children[LEFT]), Color::BLACK);
                        t = right_rotate(t, static_cast<Node *>(db->parent));
                    }
                } else if (is_black(static_cast<Node *>(db->sibling())) &&
                           is_red(static_cast<Node *>(db->sibling()->children[RIGHT]))) {
                    if (db == static_cast<Node *>(static_cast<Node *>(db->parent)->children[LEFT])) {
                        set_colors(db, Color::BLACK,
                                   static_cast<Node *>(db->parent), Color::BLACK,
                                   static_cast<Node *>(db->sibling()), static_cast<Node *>(db->parent)->color,
                                   static_cast<Node *>(db->sibling()->children[RIGHT]), Color::BLACK);
                        t = left_rotate(t, static_cast<Node *>(db->parent));
                    } else {
                        set_colors(db, Color::BLACK,
                                   static_cast<Node *>(db->parent), Color::BLACK,
                                   static_cast<Node *>(db->sibling()->children[RIGHT]),
                                   static_cast<Node *>(db->parent)->color);
                        t = left_rotate(t, static_cast<Node *>(db->sibling()));
                        t = right_rotate(t, static_cast<Node *>(db->parent));
                    }
                } else if (is_black(static_cast<Node *>(db->sibling())) &&
                           is_black(static_cast<Node *>(db->sibling()->children[LEFT])) &&
                           is_black(static_cast<Node *>(db->sibling()->children[RIGHT]))) {
                    set_colors(db, Color::BLACK,
                               static_cast<Node *>(db->sibling()), Color::RED);
                    blacken(static_cast<Node *>(db->parent));
                    db = static_cast<Node *>(db->parent);
                }
            } else {
                db->color = Color::BLACK;
                blacken(static_cast<Node *>(db->parent));
                db = static_cast<Node *>(db->parent);
            }
        }
        t->color = Color::BLACK;
        if (deb) {
            deb->replace_with(nullptr);
            factory.destroy(deb);
        }
        return t;
    }

    template<class T, class Node, class Compare, class Factory>
    Node *RbTree<T, Node, Compare, Factory>::del(Node *t, Node *x) {
        if (!x) return t;
        Node *parent = static_cast<Node *>(x->parent);
        Node *db = nullptr;        //doubly black
        Node *y;

        if (static_cast<Node *>(x->children[LEFT]) == nullptr) {
            db = static_cast<Node *>(x->children[RIGHT]);
            x->replace_with(db);
        } else if (static_cast<Node *>(x->children[RIGHT]) == nullptr) {
            db = static_cast<Node *>(x->children[LEFT]);
            x->replace_with(db);
        } else {
            y = this->min_node(static_cast<Node *>(x->children[RIGHT]));
            parent = static_cast<Node *>(y->parent);
            db = static_cast<Node *>(y->children[RIGHT]);
            x->x = y->x;
            y->replace_with(db);
            x = y;
        }
        if (x->color == Color::BLACK)
            t = delete_fix(t, make_black(parent, db), db == nullptr);
        remove(x);
        return t;
    }

    template<class T, class Node, class Compare, class Factory>
    bool RbTree<T, Node, Compare, Factory>::insert(const T &x) {
        auto new_root = insert(this->root, x);
        if (!new_root) return false;
        else {
            return this->root = new_root;
        }
    }

    template<class T, class Node, class Compare, class Factory>
    bool RbTree<T, Node, Compare, Factory>::erase(const T &x) {
        auto node = this->find_last(x);
        if (node && compare(node->x, x) == Eq) {
            n--;
            this->root = del(this->root, node);
            return true;
        }
        return false;
    }


}
#endif //DATA_STRUCTURE_FOR_LOVE_RE_TREE_HPP
