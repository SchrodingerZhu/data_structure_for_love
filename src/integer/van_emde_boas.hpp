//
// Created by schrodinger on 4/2/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_VAN_EMDE_BOAS_HPP
#define DATA_STRUCTURE_FOR_LOVE_VAN_EMDE_BOAS_HPP

#include <integer_set_base.hpp>
#include <optional>
#include <cstring>
#include <utility>

#ifdef DEBUG
#include <unordered_set>
#include <limits>
#include <iostream>
#include <sstream>

#endif //DEBUG

namespace data_structure {
    template<typename Int, size_t bit = max_bit(std::numeric_limits<Int>::max())>
    class VebTree : IntegerSetBase<Int> {
        struct NodeBase;
        template<size_t Degree>
        struct Node;
        Node<bit> *root = nullptr;
    public:
#ifdef DEBUG
        void debug() {
            root->display("root", "");
        }
#endif //DEBUG

        void insert(Int t) override;

        bool contains(Int t) const override;

        ~VebTree();

        VebTree();

        VebTree(const VebTree &that);

        VebTree(VebTree &&that) noexcept;

        VebTree(const std::initializer_list<Int> &list);

        std::optional<Int> succ(Int x) const override;

        std::optional<Int> pred(Int x) const override;

        void erase(Int x) override;

        std::optional<Int> min() const override { return root->min(); }

        std::optional<Int> max() const override { return root->max(); }

        class const_iterator;

        const_iterator begin() const;

        const_iterator end() const;

        const_iterator cbegin() const;

        const_iterator cend() const;

    };

    template<typename Int, size_t bit>
    struct VebTree<Int, bit>::NodeBase {

        virtual std::optional<Int> &max() noexcept = 0;

        virtual std::optional<Int> &min() noexcept = 0;

        virtual Int high(Int x) noexcept = 0;

        virtual Int low(Int x) noexcept = 0;

        virtual Int index(Int x, Int y) noexcept = 0;

        virtual bool contains(Int x) noexcept = 0;

        virtual void insert(Int x) noexcept = 0;

        virtual NodeBase *get_summary() noexcept = 0;

        virtual ~NodeBase() = default;
    };

    template<typename Int, size_t bit>
    template<size_t Degree>
    struct VebTree<Int, bit>::Node : NodeBase {
        Node<(Degree >> Int(1))> *cluster[1 << (Degree >> Int(1))];
        std::optional<Int> _max = std::nullopt, _min = std::nullopt;
        constexpr static size_t degree = Degree;
        Node<((degree + 1) >> Int(1))> *summary = nullptr;

        Node() {
            std::memset(cluster, 0, sizeof(cluster));
        }

        ~Node() override {
            if (summary) delete (summary);
            for (auto i: cluster) if (i) delete i;
        }


        NodeBase *get_summary() noexcept override {
            return summary;
        }

        Int high(Int x) noexcept override {
            return x >> (degree >> Int(1));
        }

        Int low(Int x) noexcept override {
            auto a = x & ((Int(1) << ((degree >> Int(1)) + 1)) - 1),
                    b = (Int(1) << (degree >> Int(1)));
            return (a < b) ? a : a ^ b;
        }

        Int index(Int x, Int y) noexcept override {
            return (x << (degree >> Int(1))) + y;
        }

        std::optional<Int> &max() noexcept override { return _max; }

        std::optional<Int> &min() noexcept override { return _min; }

        bool contains(Int x) noexcept override {
            if ((x == _min) || x == _max) return true;
            else if (degree == 1) return false;
            else {
                auto *node = cluster[high(x)];
                if (!node) return false;
                else return node->contains(low(x));
            }
        }

        inline void check_pos(size_t pos) noexcept {
            if (!cluster[pos]) {
                cluster[pos] = new Node<(degree >> Int(1))>;
            }
        }

        inline void check_summary() noexcept {
            if (!summary) {
                summary = new Node<((degree + 1) >> Int(1))>;
            }
        }

        inline void empty_insert(Int x) noexcept {
            _max = x;
            _min = x;
        }

        void insert(Int x) noexcept {
            if (_min == std::nullopt) empty_insert(x);
            else {
                if (x < _min) {
                    auto m = _min.value();
                    _min = x;
                    x = m;
                }
                if (degree > 1) {
                    check_pos(high(x));
                    check_summary();
                    if (cluster[high(x)]->_min == std::nullopt) {
                        summary->insert(high(x));
                        cluster[high(x)]->empty_insert(low(x));
                    } else cluster[high(x)]->insert(low(x));
                }
                if (x > _max) {
                    _max = x;
                }
            }
        }

        std::optional<Int> succ(Int x) {
            if (!_max || _max <= x) { return std::nullopt; }
            else if (degree == 1) {
                if (x == 0 && _max == 1) { return 1; }
                else return std::nullopt;
            } else if (_min != std::nullopt && x < _min) {
                return _min;
            } else {
                auto u = cluster[high(x)];
                auto max_low = u ? u->max() : std::nullopt;
                if (max_low != std::nullopt && low(x) < max_low) {
                    auto offset = cluster[high(x)]->succ(low(x));
                    return index(high(x), offset.value());
                } else {
                    auto succ_cluster = summary->succ(high(x));
                    if (succ_cluster == std::nullopt) return std::nullopt;
                    else {
                        auto offset = cluster[succ_cluster.value()]->min();
                        return index(succ_cluster.value(), offset.value());
                    }
                }
            }
        }

        std::optional<Int> pred(Int x) {
            if (!_min || _min >= x) { return std::nullopt; }
            else if (degree == 1) {
                if (x == 1 && _min == 0) { return 0; }
                else return std::nullopt;
            } else if (_max != std::nullopt && x > _max) {
                return _max;
            } else {
                auto u = cluster[high(x)];
                auto min_low = u ? u->min() : std::nullopt;
                if (min_low != std::nullopt && low(x) > min_low) {
                    auto offset = cluster[high(x)]->pred(low(x));
                    return index(high(x), offset.value());
                } else {
                    auto pred_cluster = summary->pred(high(x));
                    if (pred_cluster == std::nullopt) {
                        if (_min && x > _min) return _min;
                        else return std::nullopt;
                    } else {
                        auto offset = cluster[pred_cluster.value()]->max();
                        return index(pred_cluster.value(), offset.value());
                    }
                }
            }
        }

        void erase(Int x) {
            if (_min == _max && _min == x) {
                _min = _max = std::nullopt;
            } else if (degree == 1) {
                if (x == 0) _min = 1;
                else _min = 0;
                _max = _min;
            } else {
                if (x == _min) {
                    auto first_cluster = summary->min();
                    x = index(first_cluster.value(), cluster[first_cluster.value()]->min().value());
                    _min = x;
                }
                cluster[high(x)]->erase(low(x));
                if (cluster[high(x)]->min() == std::nullopt) {
                    summary->erase(high(x));
                    if (x == _max) {
                        auto summary_max = summary->max();
                        if (summary_max == std::nullopt) {
                            _max = _min;
                        } else {
                            _max = index(summary_max.value(), cluster[summary_max.value()]->max().value());
                        }
                    }
                } else if (x == _max) {
                    _max = index(high(x), cluster[high(x)]->max().value());
                }
            }
        }

#ifdef DEBUG

        void display(const std::string &prop, const std::string &before) {
                std::cout << before + prop + ", degree: " << degree << std::endl;
                if (_min)
                    std::cout << before << "min: " << _min.value() << std::endl;
                if (_max)
                    std::cout << before << "max: " << _max.value() << std::endl;
                if (summary) summary->display("summary", "    " + before);
                for (int i = 0; i < (1 << (degree >> 1)); ++i) {
                    std::stringstream ss;
                    ss << i;
                    std::string p;
                    ss >> p;
                    if (cluster[i]) cluster[i]->display(std::string("cluster #") + p, "    " + before);
                }
            }

#endif //DEBUG
    };

    template<typename Int, size_t bit>
    void VebTree<Int, bit>::insert(Int t) {
        root->insert(t);
    }

    template<typename Int, size_t bit>
    bool VebTree<Int, bit>::contains(Int t) const {
        return root->contains(t);
    }

    template<typename Int, size_t bit>
    VebTree<Int, bit>::~VebTree() {
        if (root) delete root;
        root = nullptr;
    }

    template<typename Int, size_t bit>
    VebTree<Int, bit>::VebTree() : root(new Node <bit>) {}

    template<typename Int, size_t bit>
    VebTree<Int, bit>::VebTree(const VebTree &that) : root(new Node <bit>) {
        for (auto i: that) this->insert(i);
    }

    template<typename Int, size_t bit>
    VebTree<Int, bit>::VebTree(VebTree &&that) noexcept {
        this->root = that.root;
        that.root = nullptr;
    }

    template<typename Int, size_t bit>
    VebTree<Int, bit>::VebTree(const std::initializer_list<Int> &list) : root(new Node <bit>) {
        for (auto i: list) { this->insert(i); }
    }

    template<typename Int, size_t bit>
    std::optional<Int> VebTree<Int, bit>::succ(Int x) const {
        return root->succ(x);
    }

    template<typename Int, size_t bit>
    std::optional<Int> VebTree<Int, bit>::pred(Int x) const {
        return root->pred(x);
    }

    template<typename Int, size_t bit>
    void VebTree<Int, bit>::erase(Int x) {
        root->erase(x);
    }

    template<typename Int, size_t bit>
    class VebTree<Int, bit>::const_iterator {
        std::optional<Int> t;
        const VebTree *tree;

        const_iterator(std::optional<Int> t, const VebTree *tree) : t(t), tree(tree) {}

    public:

        Int operator*() const {
            return t.value();
        }

        const_iterator &operator++() {
            if (t) {
                t = tree->succ(t.value());
            }
            return *this;
        }

        const const_iterator operator++(int) {
            if (t) {
                t = tree->succ(t.value());
            }
            return {t, tree};
        }

        const_iterator &operator--() {
            if (t) {
                t = tree->pred(t.value());
            }
            return *this;
        }

        const const_iterator operator--(int) {
            if (t) {
                t = tree->pred(t.value());
            }
            return {t, tree};
        }

        bool operator==(const const_iterator &that) const {
            return tree == that.tree && t == that.t;
        }

        bool operator!=(const const_iterator &that) const {
            return tree != that.tree || t != that.t;
        }

        friend VebTree;
    };

    template<typename Int, size_t bit>
    typename VebTree<Int, bit>::const_iterator VebTree<Int, bit>::begin() const {
        return const_iterator{root->min(), this};
    }

    template<typename Int, size_t bit>
    typename VebTree<Int, bit>::const_iterator VebTree<Int, bit>::end() const {
        return const_iterator{std::nullopt, this};
    }

    template<typename Int, size_t bit>
    typename VebTree<Int, bit>::const_iterator VebTree<Int, bit>::cbegin() const {
        return begin();
    }

    template<typename Int, size_t bit>
    typename VebTree<Int, bit>::const_iterator VebTree<Int, bit>::cend() const {
        return end();
    }

}
#endif //DATA_STRUCTURE_FOR_LOVE_VAN_EMDE_BOAS_HPP
