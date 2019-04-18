//
// Created by schrodinger on 19-4-18.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BINARY_TRIE_HPP
#define DATA_STRUCTURE_FOR_LOVE_BINARY_TRIE_HPP

#include <integer_set_base.hpp>

namespace data_structure {
    template<typename Int, size_t bit = max_bit(std::numeric_limits<Int>::max())>
    class BinaryTrie : IntegerSetBase<Int> {
        std::size_t n = 0;
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
            };
        } *root;

        struct Leaf : Node {
            Int value;

            std::optional<Int> get_value() const noexcept override { return value; }

            ~Leaf() override = default;
        };

    public:
        BinaryTrie() : root(new Path) {}

        ~BinaryTrie() {
            delete (root);
        }
        bool contains(Int t) const override {
            Int i{}, c{};
            Node *u = root;
            for (; i < bit; ++i) {
                c = (t >> (bit - i - 1)) & 1;
                if (u->links[c] == nullptr) break;
                u = u->links[c];
            }
            if (i == bit) return true;
            u = (!c) ? u->get_jump() :
                u->get_jump() ?
                u->get_jump()->links[1] : nullptr;
            return u == nullptr ? false : u->get_value() == t;
        }

        std::optional<Int> pred(Int t) const override {

        }

        std::optional<Int> succ(Int t) const override {

        }

        std::optional<Int> max() const override {

        }

        std::optional<Int> min() const override {

        }

        void insert(Int t) override {
            Int i{}, c{};
            Node *u = root;
            for (; i < bit; ++i) {
                c = (t >> (bit - i - 1)) & 1;
                if (u->links[c] == nullptr) break;
                u = u->links[c];
            }
            if (i == bit) return;
            auto pred = c ? u->get_jump() :
                        u->get_jump() ?
                        u->get_jump()->links[0] : nullptr;
            for (; i < bit; i++) {
                c = (t >> (bit - i - 1)) & 1;
                if (i != bit - 1)
                    u->links[c] = new Path;
                else u->links[c] = new Leaf;
                u->links[c]->father = reinterpret_cast<Path *>(u);
                u = u->links[c];
            }
            auto m = reinterpret_cast<Leaf *>(u);
            m->value = t;
            m->links[0] = pred;
            m->links[1] = pred ? pred->links[1] : nullptr;
            if (m->links[0]) m->links[0]->links[1] = u;
            if (m->links[1]) m->links[1]->links[0] = u;
            Path *v = u->father;
            while (v) {
                if ((!v->links[0] && (!v->get_jump() || v->get_jump()->get_value() > t)) ||
                    (!v->links[1] && (!v->get_jump() || v->get_jump()->get_value() < t))
                        )
                    v->jump = u;
                v = v->father;
            }
            n++;
        }

        void erase(Int t) override {
            Int i{};
            bool c{};
            Node *u = root;
            for (; i < bit; ++i) {
                c = (t >> (bit - i - 1)) & 1;
                if (u->links[c] == nullptr) return;
                u = u->links[c];
            }

            if (u->links[0]) u->links[0]->links[1] = u->links[1];
            if (u->links[1]) u->links[1]->links[0] = u->links[0];
            auto m = u->links[!c];
            Path *v = reinterpret_cast<Path *>(u);
            for (i = bit - 1; i >= 0; --i) {
                c = (t >> (bit - i - 1)) & 1;
                v = v->father;
                delete v->links[c];
                v->links[c] = nullptr;
                if (v->links[!c] || !i) break;
            }

            c = (t >> (bit - i - 1)) & 1;

            v->jump = m;
            v = v->father;
            if (!i) return;
            n -= 1;
            for (i -= 1; i >= 0; i -= 1) {
                c = (t >> (bit - i - 1)) & 1;
                if (v->jump == u) {
                    v->jump = m;
                }
                v = v->father;
                if (!i) break;
            }

        }


    };
}
#endif //DATA_STRUCTURE_FOR_LOVE_BINARY_TRIE_HPP
