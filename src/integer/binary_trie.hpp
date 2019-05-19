//
// Created by schrodinger on 19-4-18.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BINARY_TRIE_HPP
#define DATA_STRUCTURE_FOR_LOVE_BINARY_TRIE_HPP


#include <integer_set_base.hpp>
#include <optimized_vector.hpp>
#include <cassert>

namespace data_structure {
    template<typename Int, size_t bit = max_bit(std::numeric_limits<Int>::max())>
    class BinaryTrie : IntegerSetBase<Int> {
        std::size_t n = 0;
        struct Node;
        struct Path;
        struct Leaf;
        Node *root;
        Leaf dummy;

    public:
        BinaryTrie();

        BinaryTrie(const std::initializer_list<Int> &t);

        BinaryTrie(const BinaryTrie &t);

        BinaryTrie(BinaryTrie &&t) noexcept;

        ~BinaryTrie();

        bool contains(Int t) const override;

        std::optional<Int> pred(Int t) const override;

        std::optional<Int> succ(Int t) const override;

        std::optional<Int> max() const override;

        std::optional<Int> min() const override;

        void insert(Int t) override;

        void erase(Int t) override;

        class const_iterator;

        const_iterator begin() const;

        const_iterator end() const;

        const_iterator cbegin() const;

        const_iterator cend() const;
    };

    template<typename Int, size_t bit>
    struct BinaryTrie<Int, bit>::Node {
        Path *father = nullptr;
        Node *links[2] = {nullptr, nullptr};

        virtual Node *get_jump() const noexcept { return nullptr; }

        virtual std::optional<Int> get_value() const noexcept { return std::nullopt; }

        virtual ~Node() = default;
    };

    template<typename Int, size_t bit>
    struct BinaryTrie<Int, bit>::Path : Node {
        Node *jump = nullptr;

        Node *get_jump() const noexcept override { return jump; }

        ~Path() override {
            if (this->links[0]) delete (this->links[0]);
            if (this->links[1]) delete (this->links[1]);
#ifdef DEBUG
            assert((this->links[0] && this->links[1]) || jump || !this->father);
#endif //DEBUG
        };
    };

    template<typename Int, size_t bit>
    struct BinaryTrie<Int, bit>::Leaf : Node {
        Int value;

        std::optional<Int> get_value() const noexcept override { return value; }

        ~Leaf() override = default;
    };

    template<typename Int, size_t bit>
    BinaryTrie<Int, bit>::BinaryTrie() : root(new Path) {
        dummy.links[1] = dummy.links[0] = &dummy;
    }

    template<typename Int, size_t bit>
    BinaryTrie<Int, bit>::BinaryTrie(const std::initializer_list<Int> &t) : root(new Path) {
        dummy.links[1] = dummy.links[0] = &dummy;
        for (auto i: t) {
            this->insert(i);
        }
    }

    template<typename Int, size_t bit>
    BinaryTrie<Int, bit>::BinaryTrie(const BinaryTrie &t) : root(new Path) {
        dummy.links[1] = dummy.links[0] = &dummy;
        for (auto i: t) {
            this->insert(i);
        }
    }

    template<typename Int, size_t bit>
    BinaryTrie<Int, bit>::BinaryTrie(BinaryTrie &&t) noexcept {
        n = t.n;
        root = t.root;
        if (t.dummy.links[0]) t.dummy.links[0]->links[1] = &dummy;
        if (t.dummy.links[1]) t.dummy.links[1]->links[0] = &dummy;
        dummy.links[0] = t.dummy.links[0];
        dummy.links[1] = t.dummy.links[1];
        t.n = 0;
        t.root = nullptr;
    }

    template<typename Int, size_t bit>
    BinaryTrie<Int, bit>::~BinaryTrie() {
        if (root) delete (root);
    }

    template<typename Int, size_t bit>
    bool BinaryTrie<Int, bit>::contains(Int t) const {
        Int i{}, c{};
        Node *u = root;
        for (; i < bit; ++i) {
            c = (t >> (bit - i - 1)) & 1;
            if (u->links[c] == nullptr) break;
            u = u->links[c];
        }
        if (i == bit) return true;
        u = (!c) ? u->get_jump() : u->get_jump()->links[1];
        return u == &dummy ? false : u->get_value() == t;
    }

    template<typename Int, size_t bit>
    std::optional<Int> BinaryTrie<Int, bit>::pred(Int t) const {
        if (t <= min())
            return std::nullopt;
        Int i{}, c{};
        Node *u = root;
        for (; i < bit; ++i) {
            c = (t >> (bit - i - 1)) & 1;
            if (u->links[c] == nullptr) break;
            u = u->links[c];
        }
        if (i != bit) u = u->get_jump();
        if (u->get_value() >= t) {
            while (u->get_value() >= t && u->links[0] != &dummy) u = u->links[0];
            return u->get_value();
        } else {
            while (u->links[1] != &dummy && u->links[1]->get_value() < t) u = u->links[1];
            return u->get_value();
        }
    }

    template<typename Int, size_t bit>
    std::optional<Int> BinaryTrie<Int, bit>::succ(Int t) const {
        if (t >= max())
            return std::nullopt;
        Int i{}, c{};
        Node *u = root;
        for (; i < bit; ++i) {
            c = (t >> (bit - i - 1)) & 1;
            if (u->links[c] == nullptr) break;
            u = u->links[c];
        }
        if (i != bit) u = u->get_jump();
        if (u->get_value() <= t) {
            while (u->get_value() <= t && u->links[1] != &dummy) u = u->links[1];
            return u->get_value();
        } else {
            while (u->links[0] != &dummy && u->links[0]->get_value() > t) u = u->links[0];
            return u->get_value();
        }
    }

    template<typename Int, size_t bit>
    std::optional<Int> BinaryTrie<Int, bit>::max() const {
        if (dummy.links[0] != &dummy) return dummy.links[0]->get_value();
        else return std::nullopt;
    }

    template<typename Int, size_t bit>
    std::optional<Int> BinaryTrie<Int, bit>::min() const {
        if (dummy.links[1] != &dummy) return dummy.links[1]->get_value();
        else return std::nullopt;
    }

    template<typename Int, size_t bit>
    void BinaryTrie<Int, bit>::insert(Int t) {
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
            u->links[c]->father = static_cast<Path *>(u);
            u = u->links[c];
        }
        auto m = static_cast<Leaf *>(u);
        m->value = t;
        m->links[0] = pred;
        m->links[1] = pred->links[1];
        m->links[0]->links[1] = u;
        m->links[1]->links[0] = u;
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

    template<typename Int, size_t bit>
    void BinaryTrie<Int, bit>::erase(Int t) {
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
            v->links[c] = nullptr;
            if (v->links[!c] || !i) break;
        }
        c = (t >> (bit - i - 1)) & 1;
        auto p = static_cast<Path *>(v);
        p->jump = m[!c];
        p = p->father;
        if (!i) return;
        n -= 1;
        for (i -= 1; i >= 0; i -= 1) {
            c = (t >> (bit - i - 1)) & 1;
            if (p->jump == u) {
                p->jump = m[!c];
            }
            p = p->father;
            if (!i) break;
        }

    }

    template<typename Int, size_t bit>
    class BinaryTrie<Int, bit>::const_iterator {
        const Leaf *leaf;

        explicit const_iterator(const Leaf *leaf) : leaf(leaf) {}

    public:

        Int operator*() const {
            return leaf->get_value().value();
        }

        const_iterator &operator++() {
            if (leaf) {
                leaf = static_cast<Leaf *>(leaf->links[1]);
            }
            return *this;
        }

        const const_iterator operator++(int) {
            if (leaf) {
                leaf = static_cast<Leaf *>(leaf->links[1]);
            }
            return const_iterator{leaf};
        }

        const_iterator &operator--() {
            if (leaf) {
                leaf = static_cast<Leaf *>(leaf->links[0]);
            }
            return *this;
        }

        const const_iterator operator--(int) {
            if (leaf) {
                leaf = static_cast<Leaf *>(leaf->links[0]);
            }
            return {leaf};
        }

        bool operator==(const const_iterator &that) const {
            return leaf == that.leaf;
        }

        bool operator!=(const const_iterator &that) const {
            return leaf != that.leaf;
        }

        friend BinaryTrie;
    };

    template<typename Int, size_t bit>
    typename BinaryTrie<Int, bit>::const_iterator BinaryTrie<Int, bit>::begin() const {
        return const_iterator(static_cast<Leaf *>(dummy.links[1]));
    }

    template<typename Int, size_t bit>
    typename BinaryTrie<Int, bit>::const_iterator BinaryTrie<Int, bit>::end() const {
        return const_iterator(&dummy);
    }

    template<typename Int, size_t bit>
    typename BinaryTrie<Int, bit>::const_iterator BinaryTrie<Int, bit>::cbegin() const {
        return begin();
    }

    template<typename Int, size_t bit>
    typename BinaryTrie<Int, bit>::const_iterator BinaryTrie<Int, bit>::cend() const {
        return end();
    }
}
#endif //DATA_STRUCTURE_FOR_LOVE_BINARY_TRIE_HPP
