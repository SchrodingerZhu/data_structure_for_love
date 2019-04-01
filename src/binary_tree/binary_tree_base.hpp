//
// Created by schrodinger on 3/4/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BINARY_TREE_BASE_HPP
#define DATA_STRUCTURE_FOR_LOVE_BINARY_TREE_BASE_HPP

#include <object_pool.hpp>
#include <utility>
#include <stack>
#include <iostream>

namespace data_structure {
    struct Nothing {
    };

    template<typename Key, typename Value>
    struct WeightSumUp {
        constexpr size_t operator()(const Key &key, const Value &value, const size_t &a, const size_t &b) const {
            return a + b + 1;
        }
    };


    enum class TreeDirection : unsigned char {
        Left, Right, End, Up
    };

    template<typename Info>
    struct TreeWalker {
        virtual void
        update(Info const &left_info, Info const &info, Info const &right_info, const TreeDirection &direction) = 0;

        virtual Info result() = 0;
    };

    template<typename Key, typename Value, typename Info>
    struct TreeClimber {
        virtual void update(const Key &key, Value &value, Info &info) = 0;

        virtual void null() = 0;

        virtual TreeDirection where() = 0;

        virtual Info result() = 0;
    };

    template<typename Key, typename Value, typename Info>
    struct TreeVisitor {
        virtual void visit(const Key &key, Value &value, Info &info) = 0;
    };

    template<typename Key, typename Value, typename Info>
    struct Display : public TreeVisitor<Key, Value, Info> {
        void visit(const Key &key, Value &value, Info &info) override {
            std::cout << key << std::endl;
        }
    };

    template<typename Info>
    struct OrderWalker : public TreeWalker<Info> {
        Info accumulator{};

        void update(Info const &left_info, Info const &info, Info const &right_info,
                    const TreeDirection &direction) override {
            if (direction == TreeDirection::Right) {
                accumulator += info - right_info;
            } else if (direction == TreeDirection::End) {
                accumulator += info - right_info - 1;
            }
        }

        Info result() override {
            return accumulator;
        }
    };

    template<typename Key, typename Value, typename Info>
    struct BinaryTreeBaseNode {
        Key key;
        Value value;
        Info info;
        BinaryTreeBaseNode *lChild{}, *rChild{}, *father{};

        BinaryTreeBaseNode(const Key &key, const Value &value, const Info &info) : key(key), value(value), info(info) {}

        BinaryTreeBaseNode *least_node() {
            auto p = this;
            while (p->lChild) {
                p = p->lChild;
            }
            return p;
        }

        BinaryTreeBaseNode *most_node() {
            auto p = this;
            while (p->rChild) {
                p = p->rChild;
            }
            return p;
        }
    };


    template<typename Key, typename Value = Nothing, typename Info = size_t, Info Void = 0,
            typename Compare = std::less<Key>,
            typename Calculator = WeightSumUp<Key, Value>,
            typename Node = BinaryTreeBaseNode<Key, Value, Info>>
    class BinaryTreeBase {
    protected:
        constexpr static Compare compare{};
        constexpr static Calculator calculator{};
        ObjectPool<Node> node_pool{};
        Node *root{};

        Node *make_node(const Key &key, const Value &value, const Info &info = Void) {
            auto m = this->node_pool.construct_raw(key, value, info);
            update_node(m);
            return m;
        }

        void update_node(Node *node) {
            node->info = calculator(node->key, node->value,
                                    node->lChild ? node->lChild->info : Void, node->rChild ? node->rChild->info : Void);
        }

        Node *find_node(const Key &key) const {
            Node *p = this->root;
            while (p) {
                if (key == p->key) {
                    break;
                }
                if (this->compare(key, p->key)) {
                    p = p->lChild;
                } else {
                    p = p->rChild;
                }
            }
            return p;
        }

        Node *least_node(Node *node) const {
            while (node->lChild) {
                node = node->lChild;
            }
            return node;
        }

        Node *most_node(Node *node) const {
            while (node->rChild) {
                node = node->rChild;
            }
            return node;
        }


    public:
        virtual void insert(const Key &key, const Value &value) = 0;

        virtual bool contains(const Key &key) const = 0;

        virtual void erase(const Key &key) = 0;

        virtual Value &at(const Key &key) = 0;

        virtual Value &operator[](const Key &key) noexcept = 0;

        virtual Info &info_at(const Key &key) = 0;

        virtual const Key &least_key() const = 0;

        virtual const Key &most_key() const = 0;

        template<template<class> typename Walker, typename = std::enable_if_t<std::is_base_of_v<TreeWalker<Info>, Walker<Info>>>>
        auto walk_to(const Key &key) {
            Walker<Info> walker{};
            Node *p = this->root;
            while (p) {
                if (key == p->key) {
                    walker.update(
                            p->lChild ? p->lChild->info : Void,
                            p->info, p->rChild ? p->rChild->info : Void, TreeDirection::End);
                    break;
                } else if (this->compare(key, p->key)) {
                    walker.update(
                            p->lChild ? p->lChild->info : Void,
                            p->info, p->rChild ? p->rChild->info : Void, TreeDirection::Left);
                    p = p->lChild;
                } else {
                    walker.update(
                            p->lChild ? p->lChild->info : Void,
                            p->info, p->rChild ? p->rChild->info : Void, TreeDirection::Right);
                    p = p->rChild;
                }
            }
            return walker.result();
        }

        template<typename Climber,
                typename = std::enable_if_t<std::is_base_of_v<TreeClimber<Key, Value, Info>, Climber>>>
        auto climb() {
            Climber climber {};
            Node *p = this->root;
            while (climber.where() != TreeDirection::End) {
                if (!p) climber.null();
                else {
                    climber.update(p->key, p->key, p->info);
                }
                if (p)
                    switch (climber.where()) {
                        case TreeDirection::Left :
                            p = p->lChild;
                            break;
                        case TreeDirection::Right :
                            p = p->rChild;
                            break;
                        case TreeDirection::Up:
                            p = p->father;
                        default:
                            continue;
                    }
            }
            return climber.result();
        }

        template<typename Climber,
                typename = std::enable_if_t<std::is_base_of_v<TreeClimber<Key, Value, Info>, Climber>>>
        auto climb(Climber &climber) {
            Node *p = this->root;
            while (climber.where() != TreeDirection::End) {
                if (!p) climber.null();
                else {
                    climber.update(p->key, p->key, p->info);
                }
                if (p)
                    switch (climber.where()) {
                        case TreeDirection::Left :
                            p = p->lChild;
                            break;
                        case TreeDirection::Right :
                            p = p->rChild;
                            break;
                        case TreeDirection::Up:
                            p = p->father;
                        default:
                            continue;
                    }
            }
            return climber.result();
        }

        template<template<class, class, class> typename Visitor,
                typename = std::enable_if_t<std::is_base_of_v<TreeVisitor<Key, Value, Info>, Visitor<Key, Value, Info>>>>
        auto safe_in_order() {
            Visitor<Key, Value, Info> visitor{};
            std::stack<std::pair<Node *, bool>> stack;
            stack.push(std::make_pair(this->root, false));
            while (!stack.empty()) {
                if (!stack.top().first) stack.pop();
                else if (!stack.top().second) {
                    stack.top().second = true;
                    stack.push(std::make_pair(stack.top().first->lChild, false));
                } else {
                    auto node = stack.top().first;
                    visitor.visit(node->key, node->value, node->info);
                    auto m = stack.top().first->rChild;
                    stack.pop();
                    stack.push(std::make_pair(m, false));
                }
            }
        }

        template<template<class, class, class> typename Visitor,
                typename = std::enable_if_t<std::is_base_of_v<TreeVisitor<Key, Value, Info>, Visitor<Key, Value, Info>>>>
        auto safe_in_order(Visitor<Key, Value, Info> &visitor) {
            std::stack<std::pair<Node *, bool>> stack;
            stack.push(std::make_pair(this->root, false));
            while (!stack.empty()) {
                if (!stack.top().first) stack.pop();
                else if (!stack.top().second) {
                    stack.top().second = true;
                    stack.push(std::make_pair(stack.top().first->lChild, false));
                } else {
                    auto node = stack.top().first;
                    visitor.visit(node->key, node->value, node->info);
                    auto m = stack.top().first->rChild;
                    stack.pop();
                    stack.push(std::make_pair(m, false));
                }
            }
        }

        class iterator {
            Node *node;

            explicit iterator(Node *node) : node(node) {}

            friend BinaryTreeBase;
        public:
            iterator &operator++() {
                if (node->rChild) node = node->rChild->least_node();
                else if (node->father && node->father->lChild == node) node = node->father;
                else if (node = node->father) {
                    while (node->father && node != node->father->lChild) {
                        node = node->father;
                    }
                    node = node->father;
                } else node = nullptr;
                return *this;
            }

            const iterator operator++(int) {
                if (node->rChild) node = node->rChild->least_node();
                else if (node->father && node->father->lChild == node) node = node->father;
                else if (node = node->father) {
                    while (node->father && node != node->father->lChild) {
                        node = node->father;
                    }
                    node = node->father;
                } else node = nullptr;
                return *this;
            }

            std::tuple<const Key &, Value &, Info &> operator*() {
                return {node->key, node->value, node->info};
            }

            bool operator==(const iterator &that) {
                return this->node == that.node;
            }

            bool operator!=(const iterator &that) {
                return this->node != that.node;
            }
        };

        iterator begin() {
            return iterator(least_node(root));
        }

        iterator end() {
            return iterator(nullptr);
        }
    };


    template<typename Key, typename Value = Nothing, typename Info = size_t, Info Void = 0,
            typename Compare = std::less<Key>,
            typename Calculator = WeightSumUp<Key, Value>,
            typename Node = BinaryTreeBaseNode<Key, Value, Info>>
    class BasicBinaryTree : public BinaryTreeBase<Key, Value, Info, Void, Compare, Calculator, Node> {
        void insert_node(Node*& from, Node* node)  {
            Node **p = &from, *f = nullptr;
            while (*p) {
                f = *p;
                if (this->compare(node->key, (**p).key)) {
                    p = &(**p).lChild;
                } else {
                    p = &(**p).rChild;
                }
            }
            *p = node;
            (*p)->father = f;
            while (f) {
                this->update_node(f);
                f = f->father;
            }
        }
    public:
        void insert(const Key &key, const Value &value = Value()) override {
            insert_node(this->root, this->make_node(key, value));
        }

        bool contains(const Key &key) const override {
            return this->find_node(key);
        }

        void erase(const Key &key) override {
            Node *p = this->find_node(key);
            if (!p) return;
            else {
                if (!p->rChild) {
                    if (p->lChild) p->lChild->father = p->father;
                    if (p->father) {
                        if (p->father->lChild == p) {
                            p->father->lChild = p->lChild;
                        } else {
                            p->father->rChild = p->lChild;
                        }
                    } else this->root = p->lChild;
                } else {
                    if (p->lChild) {
                        auto least_right = this->least_node(p->rChild);
                        least_right->lChild = p->lChild;
                        p->lChild->father = least_right;
                    }
                    p->rChild->father = p->father;
                    if (p->father) {
                        if (p->father->lChild == p) {
                            p->father->lChild = p->rChild;
                        } else {
                            p->father->rChild = p->rChild;;
                        }
                    } else this->root = p->rChild;
                    auto m = p->lChild;
                    while (m != p->rChild) {
                        this->update_node(m);
                        m = m->father;
                    }
                    this->update_node(p->rChild);
                }
                auto m = p->father;
                while (m) {
                    this->update_node(m);
                    m = m->father;
                }
                this->node_pool.recycle(p);
            }
        }

        Value &at(const Key &key) override {
            Node *m = this->find_node(key);
            if (m) {
                return m->value;
            } else {
                throw std::runtime_error("no such key in tree");
            }
        }

        Value &operator[](const Key &key) noexcept override {
            Node *m = this->find_node(key);
            return m->value;
        }

        Info &info_at(const Key &key) override {
            Node *m = this->find_node(key);
            if (m) {
                return m->info;
            } else {
                throw std::runtime_error("no such key in tree");
            }
        }

        const Key &least_key() const override {
            if (!this->root) throw std::runtime_error("try to find least key in an empty tree");
            Node *m = this->least_node(this->root);
            return m->key;
        }

        const Key &most_key() const override {
            if (!this->root) throw std::runtime_error("try to find most key in an empty tree");
            Node *m = this->most_node(this->root);
            return m->key;
        }

        void absorb(BasicBinaryTree& that) {
            if (that.root == nullptr) return;
            else {
                this->node_pool.absorb(that.node_pool);
                if (this->compare(that.root->key, this->root->key)) {
                   insert_node(that.root, this->root);
                   this->root = that.root;
                } else {
                    insert_node(this->root, that.root);
                }
            }
            that.root = nullptr;
        }


    };
}
#endif //DATA_STRUCTURE_FOR_LOVE_BINARY_TREE_BASE_HPP
