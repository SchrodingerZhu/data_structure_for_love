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
#ifdef DEBUG
        size_t ALLOC = 0;
        size_t DELETE = 0;
#endif//DEBUG
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
#ifdef DEBUG
                DELETE += 1;
#endif //DEBUG
            }

        }

        ~YTreap() {

            destroy(root);
#ifdef DEBUG
            assert(ALLOC == DELETE);
#endif //DEBUG
        }

        YTreap() = default;


        YTreap(YTreap &&that) noexcept {
            root = that.root;
            that.root = nullptr;
#ifdef DEBUG
            ALLOC = that.ALLOC;
            DELETE = that.DELETE;
            that.ALLOC = 0;
            that.DELETE = 0;
#endif //DEBUG
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

        bool insert(T value) {
            auto m = locate(value);
            if (!*m.first) {
                auto temp = new TNode;
#ifdef DEBUG
                ALLOC += 1;
#endif //DEBUG
                temp->value = value;
                temp->father = m.second;
                *m.first = temp;
                bubble_up(temp);
                return true;
            }
            return false;
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

        bool erase(T value) {
            auto u = *locate(value).first;
            if (u) {
                trickle_down(u);
#ifdef DEBUG
                assert(!u->children[left] && !u->children[right]);
#endif //DEBUG
                if (u->father) {
                    if (u->father->children[left] == u) {
                        u->father->children[left] = nullptr;
                    } else {
                        u->father->children[right] = nullptr;
                    }
                }
                delete u;
#ifdef DEBUG
                DELETE += 1;
#endif //DEBUG
                if (root == u) root = nullptr;
                return true;
            }
            return false;
        }

        bool contains(T value) {
            return *locate(value).first;
        }

        void absorb_smaller(YTreap &that) noexcept {
            auto *s = new TNode();
#ifdef DEBUG
            ALLOC += 1;
#endif //DEBUG
            s->children[right] = root;
            if (root) root->father = s;
            s->children[left] = that.root;
            if (that.root) that.root->father = s;
            root = s;
            that.root = nullptr;
            trickle_down(s);
            if (s->father) {
                if (s->father->children[left] == s) {
                    s->father->children[left] = nullptr;
                } else {
                    s->father->children[right] = nullptr;
                }
            }
            delete s;
#ifdef DEBUG
            DELETE += 1;
            ALLOC += that.ALLOC;
            DELETE += that.DELETE;
            that.ALLOC = 0;
            that.DELETE = 0;
#endif //DEBUG
            if (root == s) root = nullptr;
        }

#ifdef DEBUG

        void recount(TNode *t) {
            if (!t) return;
            else {
                ALLOC += 1;
                recount(t->children[left]);
                recount(t->children[right]);
            }
        }

#endif //DEBUG

        YTreap split(T x) {
            TNode *u = *locate(x).first;
#ifdef DEBUG
            ALLOC = 0;
            DELETE = 0;
#endif //DEBUG
            if (!u) { return {}; }
            TNode s{};
            if (!u->children[right]) { u->children[right] = &s; }
            else {
                u = u->children[right];
                while (u->children[left]) u = u->children[left];
                u->children[left] = &s;
            }
            s.father = u;
            s.p = -1;
            bubble_up(&s);
            this->root = s.children[right];
            if (root) root->father = nullptr;
            YTreap another;
            another.root = s.children[left];
            if (another.root) another.root->father = nullptr;
            recount(root);
            another.recount(another.root);
            return another;
        }

        std::optional<T> min() const noexcept {
            if (!root) return std::nullopt;
            else {
                auto u = root;
                while (u->children[left]) {
                    u = u->children[left];
                }
                return u->value;
            }
        }

        std::optional<T> max() const noexcept {
            if (!root) return std::nullopt;
            else {
                auto u = root;
                while (u->children[right]) {
                    u = u->children[right];
                }
                return u->value;
            }
        }

        static TNode *max_node(TNode *n) noexcept {
            if (!n) return nullptr;
            while (n->children[right]) {
                n = n->children[right];
            }
            return n;
        }

        static TNode *min_node(TNode *n) noexcept {
            if (!n) return nullptr;
            while (n->children[left]) {
                n = n->children[left];
            }
            return n;
        }

        static TNode *pred(TNode *n) {
            if (!n) return nullptr;
            if (n->children[left]) return max_node(n->children[left]);
            else {
                while (n->father && n != n->father->children[right]) {
                    n = n->father;
                }
                return n->father;
            }
        }

        static TNode *succ(TNode *n) {
            if (!n) return nullptr;
            if (n->children[right]) return min_node(n->children[right]);
            else {
                while (n->father && n != n->father->children[left]) {
                    n = n->father;
                }
                return n->father;
            }
        }

        std::optional<T> succ(T t) {
            auto u = root;
            while (u) {
                if (u->value <= t) u = u->children[right];
                else {
                    auto p = pred(u);
                    if (p && p->value > t) u = p;
                    else break;
                }
            }
            if (u) return u->value;
            else return std::nullopt;
        }

#ifdef DEBUG

        std::vector<T> to_vec() {
            std::vector<T> m;
            to_vec(root, m);
            return m;
        }

        void to_vec(TNode *t, std::vector<T> &v) {
            if (!t) return;
            to_vec(t->children[left], v);
            v.push_back(t->value);
            to_vec(t->children[right], v);
        }

#endif //DEBUG

        std::optional<T> pred(T t) {
            auto u = root;
            while (u) {
                if (u->value >= t) u = u->children[left];
                else {
                    auto p = succ(u);
                    if (p && p->value < t) u = p;
                    else break;
                }
            }
            if (u) return u->value;
            else return std::nullopt;
        }

    };

    template<class T>
    RandomIntGen<int> YTreap<T>::generator{};

}

#endif //DATA_STRUCTURE_FOR_LOVE_YFAST_TREAP_HPP
