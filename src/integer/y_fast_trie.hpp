//
// Created by schrodinger on 19-4-29.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_Y_FAST_TRIE_HPP
#define DATA_STRUCTURE_FOR_LOVE_Y_FAST_TRIE_HPP

#include <new>
#include <integer_set_base.hpp>
#include <optimized_vector.hpp>
#include <iostream>
#include <cassert>
#include <yfast_treap.hpp>
#include <xfast_hash.hpp>

namespace data_structure {
    using namespace utils;

    template<typename Int, size_t bit = max_bit(
            std::numeric_limits<Int>::max()), template<class, size_t> typename HASH_BUILDER = STL_Builder>
    struct YFastTrie : IntegerSetBase<Int> {
        std::size_t n = 0;
        constexpr static Int top =
                max_bit(std::numeric_limits<Int>::max()) == bit ? std::numeric_limits<Int>::max() : ((1ull << bit) - 1);
        struct Path;
        struct Leaf;


        struct Node {
            Path *father = nullptr;
            Node *links[2] = {nullptr, nullptr};

            virtual Node *get_jump() const noexcept { return nullptr; }

            virtual std::optional<Int> get_value() const noexcept { return std::nullopt; }

            virtual ~Node() = default;
        };

//        constexpr static Node dummy {};

        struct Path : Node {
            Node *jump = nullptr;

            Node *get_jump() const noexcept override { return jump; }

            ~Path() override {
                if (this->links[0]) delete (this->links[0]);
                if (this->links[1]) delete (this->links[1]);
#ifdef DEBUG
                assert((this->links[0] && this->links[1]) || jump || !this->father);
#endif //DEBUG
            };
        } *root;

        struct Leaf : Node {
            Int value;
            YTreap<Int> *treap = nullptr;

            std::optional<Int> get_value() const noexcept override { return value; }

            ~Leaf() override {
                if (treap) delete treap;
            };
        } dummy;

        typename HASH_BUILDER<Node *, bit + 1>::type *maps[bit + 1]{};
#ifdef DEBUG

        void display(Node *u, std::string prefix, std::string indent = "") {
            auto p = dynamic_cast<Leaf *>(u);
            if (!p) {
                auto k = dynamic_cast<Leaf *>(u->get_jump());
                auto m = k ? k->value : -1;
                std::cout << indent << "Path: " + prefix << ", jump: " << m << std::endl;
                if (u->links[0]) {
                    display(u->links[0], prefix + "0", indent + "    ");
                }
                if (u->links[1]) {
                    display(u->links[1], prefix + "1", indent + "    ");
                }
            } else {
                std::cout << indent << "Leaf: " << p->value << std::endl;
                auto v = p->treap->to_vec();
                std::cout << indent << "Treap: " << p->value << std::endl << indent;
                for (auto i : v) {
                    std::cout << i << " ";
                }
                std::cout << std::endl;
            }
        }

        void display() {
            display(root, "");
        }

#endif

        void x_insert(Int t, YTreap<Int> *brand = nullptr) {
            Int i{}, c{};
            Node *u = root;
            for (; i < bit; ++i) {
                c = (t >> (bit - i - 1)) & 1;
                if (u->links[c] == nullptr) break;
                u = u->links[c];
            }
            if (i == bit) return;
            auto pred = c ? u->get_jump() :
                        (u->get_jump() ?
                         u->get_jump()->links[0] : &dummy);
            if (!pred) pred = &dummy;
            auto k = dynamic_cast<Path *>(u);
            if (k) k->jump = nullptr;
            for (; i < bit; i++) {
                c = (t >> (bit - i - 1)) & 1;
                if (i != bit - 1)
                    u->links[c] = new Path;
                else u->links[c] = new Leaf;
                maps[i + 1]->put(t >> (bit - i - 1), u->links[c]);
                u->links[c]->father = static_cast<Path *>(u);
                u = u->links[c];
            }
            auto m = static_cast<Leaf *>(u);
            m->value = t;
            m->links[0] = pred;
            m->links[1] = pred->links[1];
            m->links[0]->links[1] = u;
            m->links[1]->links[0] = u;
            m->treap = brand;
            Path *v = u->father;
            while (v) {
                if ((!v->links[0] && (!v->get_jump() || v->get_jump()->get_value() > t)) ||
                    (!v->links[1] && (!v->get_jump() || v->get_jump()->get_value() < t))
                        )
                    v->jump = u;
                v = v->father;
            }
        }

        void x_erase(Int t) {
            Int i{};
            bool c{};
            Node *u = root;
            for (; i < bit; ++i) {
                c = (t >> (bit - i - 1)) & 1;
                if (u->links[c] == nullptr) return;
                u = u->links[c];
            }

            u->links[0]->links[1] = u->links[1];
            u->links[1]->links[0] = u->links[0];
            Node *m[2];
            m[0] = u->links[0];
            m[1] = u->links[1];
            Node *v = u;
            for (i = bit - 1; i >= 0; --i) {
                c = (t >> (bit - i - 1)) & 1;
                v = v->father;
                delete v->links[c];
                maps[i + 1]->put(t >> (bit - i - 1), nullptr);

                v->links[c] = nullptr;
                if (v->links[!c] || !i) break;
            }
            c = (t >> (bit - i - 1)) & 1;
            auto p = static_cast<Path *>(v);
            p->jump = p->links[0] ? m[0] : m[1];
            p = p->father;
            if (!i) return;
            n -= 1;
            for (i -= 1; i >= 0; i -= 1) {
                c = (t >> (bit - i - 1)) & 1;
                if (p->jump == u) {
                    p->jump = p->links[0] ? m[0] : m[1];
                }
                p = p->father;
                if (!i) break;
            }
        }

        Leaf *locate(Int t) const {
            Int l = 0, h = bit + 1;
            Node *v, *u = root;
            while (h - l > 1) {
                Int i = (l + h) >> 1;
                if ((v = maps[i]->get(t >> (bit - i))) == nullptr) {
                    h = i;
                } else {
                    u = v;
                    l = i;
                }
            }
            if (l == bit) return static_cast<Leaf *>(u);
            Node *pred = (((t >> (bit - l - 1)) & 1) == 1)
                         ? u->get_jump() : u->get_jump()->links[0];
            if (pred->links[1] == &dummy) return nullptr;
            else return static_cast<Leaf *>(pred->links[1]);
        }

    public:
        YFastTrie() : root(new Path) {
            HASH_BUILDER<Node *, bit + 1> m(maps + bit);
            dummy.links[1] = dummy.links[0] = &dummy;
        }

        YFastTrie(const std::initializer_list<Int> t) : root(new Path) {
            HASH_BUILDER<Node *, bit + 1> m(maps + bit);
            dummy.links[1] = dummy.links[0] = &dummy;
            for (auto i: t) {
                this->insert(i);
            }
        }

        YFastTrie(const YFastTrie &t) : root(new Path) {
            HASH_BUILDER<Node *, bit + 1> m(maps + bit);
            dummy.links[1] = dummy.links[0] = &dummy;
            for (auto i: t) {
                this->insert(i);
            }
        }

        YFastTrie(YFastTrie &&t) noexcept {
            n = t.n;
            root = t.root;
            if (t.dummy.links[0]) t.dummy.links[0]->links[1] = &dummy;
            if (t.dummy.links[1]) t.dummy.links[1]->links[0] = &dummy;
            dummy.links[0] = t.dummy.links[0];
            dummy.links[1] = t.dummy.links[1];
            t.n = 0;
            t.root = nullptr;
            std::copy(t.maps, t.maps + bit + 1, maps);
            std::memset(t.maps, 0, sizeof(t.maps));
        }

        ~YFastTrie() {
            if (root) delete (root);
            for (auto i: maps) {
                if (i) {
                    utils::destroy_at(i);
                    ::operator delete(i);
                }
            }
        }

        bool contains(Int t) const override {
            auto leaf = locate(t);
            return (leaf && leaf->treap) ? leaf->treap->contains(t) : false;
//            for (auto i : *this) {
//                if (i == t) return true;
//            }
//            return false;
        }

        void insert(Int t) {
            if (!n) {
                x_insert(top, new YTreap<Int>);
            }
            auto tree = locate(t)->treap;

            if (tree->insert(t)) {
                n++;
                if (top != t && YTreap<Int>::generator() % bit == 0) {
                    auto p = new YTreap<Int>{tree->split(t)};
                    x_insert(t, p);
                }
            }

        }

        std::optional<Int> pred(Int t) const override {
            auto leaf = locate(t);
            if (leaf) {
                auto q = leaf->treap->pred(t);
                if (q) return q;
                else if (static_cast<Leaf *>(leaf->links[0])->treap) {
                    return static_cast<Leaf *>(leaf->links[0])->treap->max();
                }
            }
            return std::nullopt;
        }

        std::optional<Int> succ(Int t) const override {
            auto leaf = locate(t);
            if (leaf) {
                auto q = leaf->treap->succ(t);
                if (q) return q;
                else if (static_cast<Leaf *>(leaf->links[1])->treap) {
                    return static_cast<Leaf *>(leaf->links[1])->treap->min();
                }
            }
            return std::nullopt;
        }

        std::optional<Int> max() const override {
            auto p = dummy.links[0];
            if (p != &dummy) {
                if (!static_cast<Leaf *>(p)->treap->root) p = p->links[0];
                return static_cast<Leaf *>(p)->treap->max();
            }
            return std::nullopt;
        }

        std::optional<Int> min() const override {
            if (dummy.links[1] != &dummy) return static_cast<Leaf *>(dummy.links[1])->treap->min();
            else return std::nullopt;
        }


        void erase(Int t) override {
            Leaf *leaf = locate(t);
            bool flag = leaf->treap->erase(t);
            if (flag) {
                n--;
            }
            if (leaf->value == t && t != top) {
                static_cast<Leaf *>(leaf->links[1])->treap->absorb_smaller(*leaf->treap);
                x_erase(t);
            }

        }


        class const_iterator {
            const Leaf *leaf;
            typename YTreap<Int>::TNode *node;

            explicit const_iterator(const Leaf *leaf) : leaf(leaf), node(leaf->treap->min_node(leaf->treap->root)) {}

            explicit const_iterator(const Leaf *leaf, typename YTreap<Int>::TNode *node) : leaf(leaf), node(node) {}

        public:

            Int operator*() const {
                return node->value;
            }

            const_iterator &operator++() {
                if (leaf) {
                    node = YTreap<Int>::succ(node);
                    if (!node) {
                        leaf = static_cast<Leaf *>(leaf->links[1]);
                        if (leaf && leaf->treap) node = leaf->treap->min_node(leaf->treap->root);
                        else node = nullptr;
                    }
                }
                return *this;
            }

            const const_iterator operator++(int) {
                if (leaf) {
                    node = YTreap<Int>::succ(node);
                    if (!node) {
                        leaf = static_cast<Leaf *>(leaf->links[1]);
                        if (leaf && leaf->treap) node = leaf->treap->min_node(leaf->treap->root);
                    }

                }
                return const_iterator(leaf, node);
            }

            const_iterator &operator--() {
                if (leaf) {
                    node = YTreap<Int>::pred(node);
                    if (!node) {
                        leaf = static_cast<Leaf *>(leaf->links[0]);
                        if (leaf && leaf->treap) node = leaf->treap->max_node(leaf->treap->root);
                    }
                }
                return *this;
            }

            const const_iterator operator--(int) {
                if (leaf) {
                    node = YTreap<Int>::pred(node);
                    if (!node) {
                        leaf = static_cast<Leaf *>(leaf->links[0]);
                        if (leaf && leaf->treap) node = leaf->treap->max_node(leaf->treap->root);
                    }
                }
                return const_iterator(leaf, node);
            }

            bool operator==(const const_iterator &that) const {
                return leaf == that.leaf && node == that.node;
            }

            bool operator!=(const const_iterator &that) const {
                return leaf != that.leaf || node != that.node;
            }

            friend YFastTrie;
        };

        const_iterator begin() const {
            if (!n) end();
            return const_iterator(static_cast<Leaf *>(dummy.links[1]));
        }

        const_iterator end() const {
            return const_iterator(&dummy, nullptr);
        }

        const_iterator cbegin() const {
            return begin();
        }

        const_iterator cend() const {
            return end();
        }
    };
}
#endif //DATA_STRUCTURE_FOR_LOVE_Y_FAST_TRIE_HPP
