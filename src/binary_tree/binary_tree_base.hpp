//
// Created by schrodinger on 3/4/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BINARY_TREE_BASE_HPP
#define DATA_STRUCTURE_FOR_LOVE_BINARY_TREE_BASE_HPP

#include <node_factory.hpp>
#include <utility>
#include <stack>
#include <iostream>
// min
// max
// merge
//
namespace data_structure {

    template<class T>
    struct BinaryTreeNode {
        BinaryTreeNode *father = nullptr,
                *left = nullptr,
                *right = nullptr;
        const T key;
        using info_type = const T;

        template<typename ...Args>
        explicit BinaryTreeNode(Args &&...args) : key(std::forward<Args>(args)...) {}

        info_type &get() { return key; }
    };

    template<class T>
    struct WeightedNode : BinaryTreeNode<T> {
        std::size_t weight = 0;

        template<typename ...Args>
        explicit WeightedNode(Args &&...args) : BinaryTreeNode<T>(std::forward<Args>(args)...) {}
    };

    enum BinaryTreeOperation {
        RotateLeft,
        RotateRight,
        Insert,
        Delete,
        Merge,
        Split,
        Unlink
    };

    template<class Node>
    struct Update {
        virtual void update(Node *u) {};
    };

    template<class Node>
    struct WeightedUpdate : public Update<Node> {
        void update(Node *u) override {
            u->weight = (u->left ? u->left->weight : 0) +
                        (u->right ? u->right->weight : 0) + 1;
        };
    };

    template<typename T, class Node, class Updater, class Factory, class Compare = std::less<T>>
    class BinaryTree {
        Factory factory{};
        Node *root = nullptr;
        Updater updater{};
        Compare compare{};
        using info_type = typename Node::info_type;

        Node *find_node(Node *from, const T &key) {
            while (from) {
                if (from->key == key) return from;
                if (compare(key, from->key)) {
                    from = from->left;
                } else {
                    from = from->right;
                }
            }
            return from;
        }

        void bottom_up_update(Node *from) {
            while (from) {
                updater.update(from);
                from = from->father;
            }
        }

        virtual void unlink(Node *u) {
            Node *s, *p;
            if (u->left) {
                s = u->left;
            } else {
                s = u->right;
            }
            if (u == root) {
                root = s;
                p = nullptr;
            } else {
                p = u->father;
                if (p->left == u) {
                    p->left = s;
                } else {
                    p->right = s;
                }
            }
            if (s) { s->father = p; }
            bottom_up_update(p);
        }

        virtual Node *insert_node(Node *node) {
            Node **p = &root, *f = nullptr;
            while (*p) {
                f = *p;
                if (compare(node->key, (*p)->key)) {
                    p = &(*p)->left;
                } else {
                    p = &(*p)->right;
                }
            }
            *p = node;
            node->father = f;
            bottom_up_update(node);
            return node;
        }

        static Node *min_node(Node *node) {
            while (node && node->left) {
                node = node->left;
            }
            return node;
        }

        static Node *max_node(Node *node) {
            while (node && node->right) {
                node = node->right;
            }
            return node;
        }

        static Node *succ_node(Node *n) {
            if (!n) return nullptr;
            if (n->left) return max_node(n->left);
            else {
                while (n->father && n != n->father->right) {
                    n = n->father;
                }
                return n->father;
            }
        }

        static Node *pred_node(Node *n) {
            if (!n) return nullptr;
            if (n->right) return min_node(n->right);
            else {
                while (n->father && n != n->father->left) {
                    n = n->father;
                }
                return n->father;
            }
        }

    public:
        class iterator {
            Node *node;
        public:
            explicit iterator(Node *node) : node(node) {}

            info_type &operator*() { return node->get(); }

            info_type *operator->() { return &node->get(); }

            iterator &operator++() { return node = succ_node(node), *this; }

            const iterator operator++(int) {
                iterator p = *this;
                node = succ_node(node);
                return p;
            }

            iterator &operator--() { return node = pred_node(node), *this; }

            const iterator operator--(int) {
                iterator p = *this;
                node = pred_node(node);
                return p;
            }

        };

        virtual iterator insert(const T &t) {
            return iterator{insert_node(factory.construct(t))};
        }

        template<typename ...Args>
        iterator emplace(Args &&... args) {
            return iterator{insert_node(factory.construct(std::forward<Args>(args)...))};

        }


        virtual bool contains(const T &key) { return find_node(root, key); }

        virtual void erase(const T &key) {
            Node *node = find_node(root, key);
            if (!node->left || !node->right) {
                unlink(node);
                factory.destroy(node);
            } else {
                Node *w = node->right;
                while (w->left) {
                    w = w->left;
                }
                // father
                Node *p = w->father;
                w->father = node->father;
                node->father = p;
                if (w->father) {
                    if (w->father->left == node) {
                        w->father->left = w;
                    } else {
                        w->father->right = w;
                    }
                } else {
                    root = w;
                }
                if (node->father) {
                    if (node->father->left == w) {
                        node->father->left = node;
                    } else {
                        node->father->right = node;
                    }
                }

                p = node->left;
                node->left = w->left;
                w->left = p;

                if (node->left) {
                    node->left->father = node;
                }
                if (w->left) {
                    w->left->father = w;
                }

                p = node->right;
                node->right = w->right;
                w->right = p;

                if (node->right) {
                    node->right->father = node;
                }
                if (w->right) {
                    w->right->father = w;
                }
                unlink(node);
                factory.destroy(node);
            }
        }

        virtual info_type &operator[](const T &key) { return find_node(root, key)->get(); }

    };

    template<class T>
    using TrivialBinaryTree = BinaryTree<T, BinaryTreeNode<T>,
            Update<BinaryTreeNode<T>>, utils::TrivialFactory<BinaryTreeNode<T>>>;

}
#endif //DATA_STRUCTURE_FOR_LOVE_BINARY_TREE_BASE_HPP
