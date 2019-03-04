//
// Created by schrodinger on 3/4/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BINARY_TREE_BASE_HPP
#define DATA_STRUCTURE_FOR_LOVE_BINARY_TREE_BASE_HPP

#include <object_pool.hpp>
#include <utility>
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

    template<typename Key>
    struct Display {
        template<typename ...Others>
        void operator()(Key const &key, Others const &... others) const {
            std::cout << key << std::cout;
        }
    };
    enum TreeDirection: unsigned char {
        Left, Right, End
    };
    template<typename Info>
    struct OrderWalker {
        Info accumulator {};
        void update(Info const &left_info, Info const &info, Info const &right_info, const TreeDirection& direction)  {
            if (direction == Right) {
                accumulator += info - right_info;
            } else if (direction == End) {
                accumulator += info - right_info - 1;
            }
        }
        Info result() {
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
    };

    template<typename Key, typename Value = Nothing, typename Info = size_t, Info Void = 0,
            typename Compare = std::less<Key>,
            typename Calculator = WeightSumUp<Key, Value>,
            typename Node = BinaryTreeBaseNode<Key, Value, Info>>
    class BinaryTreeBase {
        constexpr static Compare compare{};
        constexpr static Calculator calculator{};
        ObjectPool<Node> node_pool{};
        Node *root{};

        Node *make_node(const Key &key, const Value &value, const Info &info = Void) {
            auto m = node_pool.construct_raw(key, value, info);
            update_node(m);
            return m;
        }

        void update_node(Node *node) {
            node->info = calculator(node->key, node->value,
                                    node->lChild ? node->lChild->info : Void, node->rChild ? node->rChild->info : Void);
        }

        Node *find_node(const Key &key) const {
            Node *p = root;
            while(p) {
                if (key == p->key) {
                    break;
                }
                if (compare(key, p->key)) {
                    p = p->lChild;
                } else {
                    p = p->rChild;
                }
            }
            return p;
        }

        Node *least_node(Node* node) const {
            while(node->lChild) {
                node = node->lChild;
            }
            return node;
        }

        Node *most_node(Node* node) const {
            while(node->rChild) {
                node = node->rChild;
            }
            return node;
        }

    public:
        void insert(const Key &key, const Value &value = Value()) {
            Node **p = &root, *f = nullptr;
            while (*p) {
                f = *p;
                if (compare(key, (**p).key)) {
                    p = &(**p).lChild;
                } else {
                    p = &(**p).rChild;
                }
            }
            *p = make_node(key, value);
            (*p)->father = f;
            while (f) {
                update_node(f);
                f = f->father;
            }
        }

        bool contains(const Key& key) const {
            return find_node(key);
        }

        void erase(const Key &key) {
            Node* p = find_node(key);
            if (!p) return;
            else {
                if(!p->rChild) {
                    if(p->lChild) p->lChild->father = p->father;
                    if(p->father) {
                        if(p->father->lChild == p) {
                            p->father->lChild = p->lChild;
                        } else {
                            p->father->rChild = p->lChild;
                        }
                    }
                    else root = p->lChild;
                }
                else {
                    if (p->lChild) {
                        auto least_right = least_node(p->rChild);
                        least_right->lChild = p->lChild;
                        p->lChild->father = least_right;
                    }
                    p->rChild->father = p->father;
                    if(p->father) {
                        if(p->father->lChild == p) {
                            p->father->lChild = p->rChild;
                        } else {
                            p->father->rChild = p->rChild;;
                        }
                    }
                    else root = p->rChild;
                    auto m = p->lChild;
                    while(m != p->rChild) {
                        update_node(m);
                        m = m->father;
                    }
                    update_node(p->rChild);
                }
                auto m = p->father;
                while(m){
                    update_node(m);
                    m = m->father;
                }
                node_pool.recycle(p);
            }
        }

        Value& at(const Key& key) {
            Node* m = find_node(key);
            if(m) {
                return m->value;
            } else {
                throw std::runtime_error("no such key in tree");
            }
        }

        Value& operator[](const Key& key) noexcept {
            Node* m = find_node(key); return m->value;
        }

        Info& info_at(const Key& key) {
            Node* m = find_node(key);
            if(m) {
                return m->info;
            } else {
                throw std::runtime_error("no such key in tree");
            }
        }

        const Key& least_key() const {
            if(!root) throw std::runtime_error("try to find least key in an empty tree");
            Node* m = least_node(root);
            return m->key;
        }

        const Key& most_key() const {
            if(!root) throw std::runtime_error("try to find most key in an empty tree");
            Node* m = most_node(root);
            return m->key;
        }

        template <typename Walker>
        auto walk_to(const Key& key) {
            Walker walker {};
            Node *p = root;
            while (p) {
                if (key == p->key) {
                    walker.update(
                            p->lChild ? p->lChild->info : Void,
                            p->info, p->rChild ? p->rChild->info : Void, End);
                    break;
                }
                else if (compare(key, p->key)) {
                    walker.update(
                            p->lChild ? p->lChild->info : Void,
                            p->info,p->rChild ? p->rChild->info : Void, Left);
                    p = p->lChild;
                } else {
                    walker.update(
                            p->lChild ? p->lChild->info : Void,
                            p->info,p->rChild ? p->rChild->info : Void, Right);
                    p = p->rChild;
                }
            }
            return walker.result();
        }
    };
}
#endif //DATA_STRUCTURE_FOR_LOVE_BINARY_TREE_BASE_HPP
