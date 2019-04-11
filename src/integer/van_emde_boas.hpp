//
// Created by schrodinger on 4/2/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_VAN_EMDE_BOAS_HPP
#define DATA_STRUCTURE_FOR_LOVE_VAN_EMDE_BOAS_HPP

#include <integer_set_base.hpp>
#include <array>
#include <optional>
#include <utility>
#include <unordered_map>

namespace data_structure {
    template<typename Int, size_t bit = 8 * sizeof(Int)>
    class VebTree {
        struct NodeBase {
            VebTree *veb = nullptr;

            virtual std::optional<Int> &max() noexcept = 0;

            virtual std::optional<Int> &min() noexcept = 0;

            virtual size_t degree() const noexcept = 0;

            virtual Int high(Int x) noexcept = 0;

            virtual Int low(Int x) noexcept = 0;

            virtual Int index(Int x, Int y) noexcept = 0;

            virtual bool contains(Int x) = 0;

            virtual void insert(Int x) = 0;

            virtual NodeBase *get_summary() = 0;

            virtual ~NodeBase() = default;
        };

        struct hash {
            size_t operator()(const std::pair<NodeBase *, size_t> &t) const {
                return reinterpret_cast<size_t>(t.first) ^ t.second;
            }
        };

        std::unordered_map<std::pair<NodeBase *, size_t>, NodeBase *, hash> address_map;

        template<size_t Degree>
        struct Node : NodeBase {
            //constexpr static size_t _degree = Degree;
            std::optional<Int> _max = std::nullopt, _min = std::nullopt;

            Node<((Degree + 1) >> Int(1))> *summary = nullptr;

            ~Node() override {
                if (summary) delete (summary);
            }

            void cluster(size_t i, Node<(Degree >> Int(1))> *t) {
                this->veb->address_map[std::pair{this, i}] = t;
            }

            Node<(Degree >> 1u)> *cluster(size_t i) {
                return reinterpret_cast<Node<(Degree >> Int(1))> *>(this->veb->address_map[std::pair{this, i}]);
            }

            NodeBase *get_summary() override {
                return summary;
            }

            explicit Node(VebTree *veb) { this->veb = veb; }

            Int high(Int x) noexcept override {
                return x >> (Degree >> Int(1));
            }

            Int low(Int x) noexcept override {
                auto a = x & ((Int(1) << ((Degree >> Int(1)) + 1)) - 1),
                        b = (Int(1) << (Degree >> Int(1)));
                return (a < b) ? a : a | b;
            }

            Int index(Int x, Int y) noexcept override {
                return (x << (Degree >> Int(1))) + y;
            }

            std::optional<Int> &max() noexcept override { return _max; }

            std::optional<Int> &min() noexcept override { return _min; }

            size_t degree() const noexcept override { return Degree; }

            bool contains(Int x) override {
                if ((x == _min) || x == _max) return true;
                else if (Degree == 1) return false;
                else {
                    auto *node = cluster(high(x));
                    if (!node) return false;
                    else return node->contains(low(x));
                }
            }

            void check_pos(size_t pos) {
                if (!cluster(pos)) {
                    cluster(pos, new Node<(Degree >> Int(1))>{this->veb});
                }
            }

            void check_summary() {
                if (!summary) {
                    summary = new Node<((Degree + 1) >> Int(1))>{this->veb};
                }
            }

            void empty_insert(Int x) {
                _max = x;
                _min = x;
            }

            void insert(Int x) {
                if (_min == std::nullopt) empty_insert(x);
                else {
                    if (x < _min) {
                        auto m = _min.value();
                        _min = x;
                        x = m;
                    }
                    if (Degree > 1) {
                        check_pos(high(x));
                        check_summary();
                        if (cluster(high(x))->_min == std::nullopt) {
                            summary->insert(high(x));
                            cluster(high(x))->empty_insert(low(x));
                        } else cluster(high(x))->insert(low(x));
                    }
                    if (x > _max) {
                        _max = x;
                    }
                }
            }

            std::optional<Int> succ(Int x) {
                if (!_max || _max <= x) { return std::nullopt; }
                else if (Degree == 1) {
                    if (x == 0 && _max == 1) { return 1; }
                    else return std::nullopt;
                } else if (_min != std::nullopt && x < _min) {
                    return _min;
                } else {
                    auto max_low = cluster(high(x))->max();
                    if (max_low != std::nullopt && low(x) < max_low) {
                        auto offset = cluster(high(x))->succ(low(x));
                        return index(high(x), offset.value());
                    } else {
                        auto succ_cluster = summary->succ(high(x));
                        if (succ_cluster == std::nullopt) return std::nullopt;
                        else {
                            auto offset = cluster(succ_cluster.value())->min();
                            return index(succ_cluster.value(), offset.value());
                        }
                    }
                }
            }

            std::optional<Int> pred(Int x) {
                if (!_min || _min >= x) { return std::nullopt; }
                else if (Degree == 1) {
                    if (x == 1 && _min == 0) { return 0; }
                    else return std::nullopt;
                } else if (_max != std::nullopt && x > _max) {
                    return _max;
                } else {
                    auto min_low = cluster(high(x))->min();
                    if (min_low != std::nullopt && low(x) > min_low) {
                        auto offset = cluster(high(x))->pred(low(x));
                        return index(high(x), offset.value());
                    } else {
                        auto pred_cluster = summary->pred(high(x));
                        if (pred_cluster == std::nullopt) {
                            if (_min && x > _min) return _min;
                            else return std::nullopt;
                        } else {
                            auto offset = cluster(pred_cluster.value())->max();
                            return index(pred_cluster.value(), offset.value());
                        }
                    }
                }
            }

            void erase(Int x) {
                if (_min == _max) {
                    _min = _max = std::nullopt;
                } else if (Degree == 1) {
                    if (x == 0) _min = 1;
                    else _min = 0;
                    _max = _min;
                } else {
                    if (x == _min) {
                        auto first_cluster = summary->min();
                        x = index(first_cluster.value(), cluster(first_cluster.value())->min().value());
                        _min = x;
                    }
                    cluster(high(x))->erase(low(x));
                    if (cluster(high(x))->min() == std::nullopt) {
                        summary->erase(high(x));
                        if (x == _max) {
                            auto summary_max = summary->max();
                            if (summary_max == std::nullopt) {
                                _max = _min;
                            } else {
                                _max = index(summary_max.value(), cluster(summary_max.value())->max().value());
                            }
                        }
                    } else if (x == _max) {
                        _max = index(high(x), cluster(high(x))->max().value());
                    }
                }
            }


        };

        Node<bit - 1> *root = nullptr;

        template<class N>
        void update_veb(N *n) {
            n->veb = this;
            if (n->get_summary()) update_veb(n->get_summary());
        }

    public:
        void insert(Int t) {
            root->insert(t);
        }

        bool contains(Int t) {
            return root->contains(t);
        }

        ~VebTree() {
            if (root) delete root;
            for (const auto &i: address_map) { delete i.second; }
        }

        VebTree() : root(new Node<bit - 1>(this)) {};

        VebTree(const VebTree &that) : root(new Node<bit - 1>(this)) {
            for (auto i: that) this->insert(i);
        }

        VebTree(VebTree &&that) noexcept {
            this->root = that.root;
            this->root->veb = this;
            for (const auto &i: that.address_map) {
                update_veb(i.second);
                address_map.insert(i);
            }
            that.root = nullptr;
            that.address_map.clear();
        }

        std::optional<Int> succ(Int x) const {
            return root->succ(x);
        }

        std::optional<Int> pred(Int x) const {
            return root->pred(x);
        }

        void erase(Int x) {
            root->erase(x);
        }

        class const_iterator {
            std::optional<Int> t;
            const VebTree *tree;

            const_iterator(std::optional<Int> t, const VebTree *tree) : t(t), tree(tree) {}

        public:
            Int operator*() {
                return t.value();
            }

            const_iterator &operator++() {
                if (t) {
                    t = tree->succ(t.value());
                }
                return *this;
            }

            const const_iterator operator++(int) {
                return {tree->succ(t.value()), tree};
            }

            const_iterator &operator--() {
                if (t) {
                    t = tree->pred(t.value());
                }
                return *this;
            }

            const const_iterator operator--(int) {
                return {tree->pred(t.value()), tree};
            }

            bool operator==(const const_iterator &that) const {
                return tree == that.tree && t == that.t;
            }

            bool operator!=(const const_iterator &that) const {
                return tree != that.tree || t != that.t;
            }

            friend VebTree;
        };

        using iterator = const_iterator;

        iterator begin() const {
            return const_iterator{root->min(), this};
        }

        iterator end() const {
            return const_iterator{std::nullopt, this};
        }

        iterator cbegin() const {
            return begin();
        }

        iterator cend() const {
            return end();
        }
    };


}
#endif //DATA_STRUCTURE_FOR_LOVE_VAN_EMDE_BOAS_HPP
