//
// Created by schrodinger on 19-4-29.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_YFAST_TREAP_HPP
#define DATA_STRUCTURE_FOR_LOVE_YFAST_TREAP_HPP

#include <static_random_helper.hpp>
#include <random>

#ifdef DEBUG
#include <cassert>
#endif
namespace data_structure::utils {

    template<class T>
    struct YTreap {
        static RandomIntGen<int> generator;

        enum DIRECTION : bool {
            left = 0, right = 1
        };

        struct TNode {
            TNode *children[2] = {nullptr, nullptr};
            TNode *father = nullptr;
            T value;
            int p;

            TNode() : p(generator()) {};
        } *root = nullptr;

        void destroy(TNode *u) {
            if (u) {
                destroy(u->children[left]);
                destroy(u->children[right]);
                delete u;
            }

        }

        ~YTreap() {
            destroy(root);
        }

        void rotate(TNode *u, DIRECTION direction) {
            TNode *w = u->children[!direction];
            w->father = u->father;
            if (w->father) {
                if (w->father->children[left] == u) w->father->children[left] = w;
                else w->father->children[right] = w;
            }
            u->children[!direction] = w->children[direction];
            if (u->children[!direction]) {
                u->children[!direction]->father = u;
            }
            u->father = w;
            w->children[direction] = u;
            if (u == root) {
                root = w;
                root->father = nullptr;
            }
        }

        void bubble_up(TNode *u) {
            while (u->father && u->father->p > u->p) {
                if (u->father->children[left] == u) {
                    rotate(u->father, right);
                } else {
                    rotate(u->father, left);
                }
            }
            if (!u->father) { root = u; }
        }

        std::pair<TNode **, TNode *> locate(T value) {
            TNode **p = &root, *f = nullptr;
            while (*p) {
                f = *p;
                if ((*p)->value == value) break;
                else if ((*p)->value > value) p = &(*p)->children[left];
                else p = &(*p)->children[right];
            }
            return {p, f};
        }

        void insert(T value) {
            auto m = locate(value);
            if (!*m.first) {
                auto temp = new TNode;
                temp->value = value;
                temp->father = m.second;
                *m.first = temp;
                bubble_up(temp);
            }
        }

        void trickle_down(TNode *u) {
            while (u->children[left] || u->children[right]) {
                if (!u->children[left]) {
                    rotate(u, left);
                } else if (!u->children[right]) {
                    rotate(u, right);
                } else if (u->children[left]->p < u->children[right]->p) {
                    rotate(u, right);
                } else {
                    rotate(u, left);
                }
                if (root == u) {
                    root = u->father;
                }
            }
        }

        void remove(T value) {
            auto p = locate(value);
            if (*p.first) {
                trickle_down(*p.first);
#ifdef DEBUG
                assert(!(*p.first)->children[left] && !(*p.first)->children[right]);
#endif //DEBUG
                if (p.second) {
                    if (p.second->children[left] == *p.first) {
                        p.second->children[left] = nullptr;
                    } else {
                        p.second->children[right] = nullptr;
                    }
                }
                delete *p.first;
            }
        }

        bool contains(T value) {
            return *locate(value).first;
        }

    };

    template<class T>
    RandomIntGen<int> YTreap<T>::generator{};

}

#endif //DATA_STRUCTURE_FOR_LOVE_YFAST_TREAP_HPP
