//
// Created by schrodinger on 19-4-18.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_X_FAST_TRIE_TRIE_HPP
#define DATA_STRUCTURE_FOR_LOVE_X_FAST_TRIE_TRIE_HPP

#include <new>
#include <integer_set_base.hpp>
#include <optimized_vector.hpp>
#include <cassert>
#include <xfast_hash.hpp>

namespace data_structure {
    using namespace utils;

    template<typename Int, size_t bit = max_bit(
            std::numeric_limits<Int>::max()), template<class, size_t> typename HASH_BUILDER = STL_Builder>
    class XFastTrie : IntegerSetBase<Int> {
        std::size_t n = 0;
        struct Node;
        struct Path;
        struct Leaf;
        Path *root;
        Leaf dummy;
        typename HASH_BUILDER<Node *, bit + 1>::type *maps[bit + 1]{};
    public:
        XFastTrie();

        XFastTrie(const std::initializer_list<Int> &t);

        XFastTrie(const XFastTrie &t);

        XFastTrie(XFastTrie &&t) noexcept;

        ~XFastTrie();

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

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    struct XFastTrie<Int, bit, HASH_BUILDER>::Node {
        Path *father = nullptr;
        Node *links[2] = {nullptr, nullptr};

        virtual Node *get_jump() const noexcept { return nullptr; }

        virtual std::optional<Int> get_value() const noexcept { return std::nullopt; }

        virtual ~Node() = default;
    };

//        constexpr static Node dummy {};

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    struct XFastTrie<Int, bit, HASH_BUILDER>::Path : Node {
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

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    struct XFastTrie<Int, bit, HASH_BUILDER>::Leaf : Node {
        Int value;

        std::optional<Int> get_value() const noexcept override { return value; }

        ~Leaf() override = default;
    };

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    XFastTrie<Int, bit, HASH_BUILDER>::XFastTrie() : root(new Path) {
        HASH_BUILDER<Node *, bit + 1> m(maps + bit);
        dummy.links[1] = dummy.links[0] = &dummy;
    }

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    XFastTrie<Int, bit, HASH_BUILDER>::XFastTrie(const std::initializer_list<Int> &t) : root(new Path) {
        HASH_BUILDER<Node *, bit + 1> m(maps + bit);
        dummy.links[1] = dummy.links[0] = &dummy;
        for (auto i: t) {
            this->insert(i);
        }
    }

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    XFastTrie<Int, bit, HASH_BUILDER>::XFastTrie(const XFastTrie &t) : root(new Path) {
        HASH_BUILDER<Node *, bit + 1> m(maps + bit);
        dummy.links[1] = dummy.links[0] = &dummy;
        for (auto i: t) {
            this->insert(i);
        }
    }

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    XFastTrie<Int, bit, HASH_BUILDER>::XFastTrie(XFastTrie &&t) noexcept {
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

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    XFastTrie<Int, bit, HASH_BUILDER>::~XFastTrie() {
        if (root) delete (root);
        for (auto i: maps) {
            if (i) {
                utils::destroy_at(i);
                ::operator delete(i);
            }
        }
    }

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    bool XFastTrie<Int, bit, HASH_BUILDER>::contains(Int t) const {
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
        if (l == bit) return u->get_value() == t;
        Node *pred = (((t >> (bit - l - 1)) & 1) == 1)
                     ? u->get_jump() : u->get_jump()->links[0];
        return (pred->links[1] == &dummy) ? false : pred->links[1]->get_value() == t;
    }

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    std::optional<Int> XFastTrie<Int, bit, HASH_BUILDER>::pred(Int t) const {
        if (t <= min())
            return std::nullopt;
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
        if (l != bit) u = u->get_jump();
        if (u->get_value() >= t) {
            while (u->get_value() >= t && u->links[0] != &dummy) u = u->links[0];
            return u->get_value();
        } else {
            while (u->links[1] != &dummy && u->links[1]->get_value() < t) u = u->links[1];
            return u->get_value();
        }
    }

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    std::optional<Int> XFastTrie<Int, bit, HASH_BUILDER>::succ(Int t) const {
        if (t >= max())
            return std::nullopt;
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
        if (l != bit) u = u->get_jump();
        if (u->get_value() <= t) {
            while (u->get_value() <= t && u->links[1] != &dummy) u = u->links[1];
            return u->get_value();
        } else {
            while (u->links[0] != &dummy && u->links[0]->get_value() > t) u = u->links[0];
            return u->get_value();
        }
    }

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    std::optional<Int> XFastTrie<Int, bit, HASH_BUILDER>::max() const {
        if (dummy.links[0] != &dummy) return dummy.links[0]->get_value();
        else return std::nullopt;
    }

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    std::optional<Int> XFastTrie<Int, bit, HASH_BUILDER>::min() const {
        if (dummy.links[1] != &dummy) return dummy.links[1]->get_value();
        else return std::nullopt;
    }

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    void XFastTrie<Int, bit, HASH_BUILDER>::insert(Int t) {
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

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    void XFastTrie<Int, bit, HASH_BUILDER>::erase(Int t) {
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

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    class XFastTrie<Int, bit, HASH_BUILDER>::const_iterator {
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

        friend XFastTrie;
    };

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    typename XFastTrie<Int, bit, HASH_BUILDER>::const_iterator XFastTrie<Int, bit, HASH_BUILDER>::begin() const {
        return const_iterator(static_cast<Leaf *>(dummy.links[1]));
    }

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    typename XFastTrie<Int, bit, HASH_BUILDER>::const_iterator XFastTrie<Int, bit, HASH_BUILDER>::end() const {
        return const_iterator(&dummy);
    }

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    typename XFastTrie<Int, bit, HASH_BUILDER>::const_iterator XFastTrie<Int, bit, HASH_BUILDER>::cbegin() const {
        return begin();
    }

    template<typename Int, size_t bit, template<class, size_t> typename HASH_BUILDER>
    typename XFastTrie<Int, bit, HASH_BUILDER>::const_iterator XFastTrie<Int, bit, HASH_BUILDER>::cend() const {
        return end();
    }
}
#endif //DATA_STRUCTURE_FOR_LOVE_X_FAST_TRIE_TRIE_HPP
