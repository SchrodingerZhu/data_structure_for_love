//
// Created by schrodinger on 19-5-16.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_SCAPEGOAT_HPP
#define DATA_STRUCTURE_FOR_LOVE_SCAPEGOAT_HPP

#include <binary_tree_base.hpp>
#include <cmath>

namespace data_structure {
    template<class T, class Node = WeightedBSTNode<T>,
            class Compare = utils::DefaultCompare<T>, class Factory = utils::PoolFactory<Node>>
    class ScapeGoat : public BSTree<T, Node, Compare, Factory> {
    protected:
        using BinTree<Node, Factory>::root;
        using BinTree<Node, Factory>::factory;
        using BSTree<T, Node, Compare, Factory>::compare;
        using BSTree<T, Node, Compare, Factory>::n;
        size_t q = 0;

        size_t pack(Node *u, Node **array, size_t i);

        Node *rebuild(Node **a, size_t i, size_t ns);

        void rebuild(Node *u);

        constexpr inline static size_t factor(size_t q);

        constexpr static double __factor = 2.4663034623764317;

        std::tuple<size_t, Node **, Node *> find_position(const T &x);

    public:
        virtual bool insert(const T &x);

        virtual bool erase(const T &x);
    };

    template<class T, class Node, class Compare, class Factory>
    void ScapeGoat<T, Node, Compare, Factory>::rebuild(Node *u) {
        size_t ns = u->weight;
        Node *p = static_cast<Node *>(u->parent);
        Node *array[ns];
        pack(u, array, 0);
        if (!p) {
            root = rebuild(array, 0, ns);
            root->parent = nullptr;
        } else if (p->children[RIGHT] == u) {
            p->children[RIGHT] = rebuild(array, 0, ns);
            p->children[RIGHT]->parent = p;
        } else {
            p->children[LEFT] = rebuild(array, 0, ns);
            p->children[LEFT]->parent = p;
        }
        if (p) p->update();
    }

    template<class T, class Node, class Compare, class Factory>
    size_t ScapeGoat<T, Node, Compare, Factory>::pack(Node *u, Node **array, size_t i) {
        if (!u) return i;
        i = pack(static_cast<Node *>(u->children[LEFT]), array, i);
        array[i++] = u;
        return pack(static_cast<Node *>(u->children[RIGHT]), array, i);
    }

    template<class T, class Node, class Compare, class Factory>
    Node *ScapeGoat<T, Node, Compare, Factory>::rebuild(Node **a, size_t i, size_t ns) {
        if (!ns) return nullptr;
        size_t m = ns >> 1u;
        a[i + m]->children[LEFT] = rebuild(a, i, m);
        if (a[i + m]->children[LEFT]) {
            a[i + m]->children[LEFT]->parent = a[i + m];
        }
        a[i + m]->children[RIGHT] = rebuild(a, i + m + 1, ns - m - 1);
        if (a[i + m]->children[RIGHT]) {
            a[i + m]->children[RIGHT]->parent = a[i + m];
        }
        a[i + m]->update();
        return a[i + m];
    }

    template<class T, class Node, class Compare, class Factory>
    constexpr inline size_t ScapeGoat<T, Node, Compare, Factory>::factor(size_t q) {
        return (int) (__factor * std::log(q));
    }

    template<class T, class Node, class Compare, class Factory>
    bool ScapeGoat<T, Node, Compare, Factory>::insert(const T &x) {
        auto pos = find_position(x);
        if (!std::get<1>(pos)) return false;
        else {
            Node *u = factory.construct(x);
            *std::get<1>(pos) = u;
            u->parent = std::get<2>(pos);
            this->bottom_up_update(u);
            if (std::get<0>(pos) > factor(q)) {
                auto w = static_cast<Node *>(u->parent);
                while (w->parent && w->parent->parent && 3 * w->weight <= 2 * static_cast<Node *>(w->parent)->weight) {
                    w = static_cast<Node *>(w->parent);
                }
                rebuild(static_cast<Node *>(w->parent));
            }
        }
        return true;
    }

    template<class T, class Node, class Compare, class Factory>
    std::tuple<size_t, Node **, Node *> ScapeGoat<T, Node, Compare, Factory>::find_position(const T &x) {
        Node *w = root;
        if (w == nullptr) {
            n++;
            q++;
            return {0, &root, nullptr};
        }
        bool done = false;
        size_t d = 0;
        Node **result = nullptr, *father = nullptr;
        do {
            auto res = compare(x, w->x);
            if (res == utils::Less) {
                if (w->children[LEFT] == nullptr) {
                    father = w;
                    result = reinterpret_cast<Node **>(&w->children[LEFT]);
                    done = true;
                } else {
                    w = static_cast<Node *>(w->children[LEFT]);
                }
            } else if (res == utils::Greater) {
                if (w->children[RIGHT] == nullptr) {
                    father = w;
                    result = reinterpret_cast<Node **>(&w->children[RIGHT]);
                    done = true;
                }
                w = static_cast<Node *>(w->children[RIGHT]);
            } else {
                return {d, nullptr, nullptr};
            }
            d++;
        } while (!done);
        n++;
        q++;
        return {d, result, father};
    }

    template<class T, class Node, class Compare, class Factory>
    bool ScapeGoat<T, Node, Compare, Factory>::erase(const T &x) {
        if (BSTree<T, Node, Compare, Factory>::erase(x)) {
            if (!root) {
                q = 0;
            } else if ((n << 1) < q) {
                rebuild(root);
                q = n;
            }
            return true;
        }
        return false;
    }
}
#endif //DATA_STRUCTURE_FOR_LOVE_SCAPEGOAT_HPP
