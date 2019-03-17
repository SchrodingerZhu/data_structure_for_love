#include <iostream>
#include <cstdio>
#include <memory>
#include <object_pool.hpp>
namespace data_structure {

    template <class T>
    struct XorNode { // A XorNode<T>.
        T value; // The Key.
        XorNode *link = nullptr; // The pointer prev ^ the pointer next
    };

    template <class T, class Alloc = ObjectPool<XorNode<T>>>
    class XorList { // A linkedlist
        Alloc memory_pool;

    public:
        XorNode<T> *head = nullptr; // Head Pointer
        XorNode<T> *tail = nullptr; // Tail Pointer

        XorNode<T> *combine(XorNode<T> *a, XorNode<T> *b) {  // XorNode<T>* ^ XorNode<T>*
            return reinterpret_cast<XorNode<T> *>(reinterpret_cast<ptrdiff_t>(a) ^ reinterpret_cast<ptrdiff_t>(b));
        }

        void insert(const T& newValue) { // Add a XorNode<T> at the end.
            auto *newNode = memory_pool.allocate(1);
            memory_pool.construct(newNode);
            std::allocator<XorNode<T>> test;
            newNode->value = newValue;
            if (head == nullptr) {
                head = newNode;
                tail = newNode;
            } else {
                newNode->link = combine(tail, nullptr);
                tail->link = combine(combine(tail->link, nullptr), newNode);
                tail = newNode;
            }
        }

        void view() { // Print out the list.
            if (head == nullptr)
                return;
            viewList(head, nullptr);
        }

        void viewList(XorNode<T> *now, XorNode<T> *last) {
            printf("%s ", now->value);
            if (now == tail) {
                //cout << endl;
                return;
            }
            viewList(combine(now->link, last), now);
        }

        void erase(const T& target) { // Delete a XorNode<T>.
            if (head == nullptr)
                return;
            deleteTarget(head, nullptr, target);
        }

        void deleteTarget(XorNode<T> *now, XorNode<T> *last, const T& target) {
            if (now->value == target) {
                if (now == tail && now == head) {
                    tail = nullptr;
                    head = nullptr;
                    return;
                }
                if (now == tail) {
                    last->link = combine(combine(last->link, now), nullptr);
                    tail = last;
                    return;
                }
                XorNode<T> *next = combine(now->link, last);
                if (now == head)
                    head = next;
                else
                    last->link = combine(combine(last->link, now), next);
                next->link = combine(combine(next->link, now), last);
                now = next;
            }
            if (now == tail)
                return;
            deleteTarget(combine(now->link, last), now, target);
        }

        bool find(const T& target) { // Find out if a XorNode<T> exists.
            if (head == nullptr)
                return false;
            return findTarget(head, nullptr, target);
        }

        bool findTarget(XorNode<T> *now, XorNode<T> *last, const T& target) {
            if (now->value == target)
                return true;
            if (now == tail)
                return false;
            findTarget(combine(now->link, last), now, target);
        }

    };

}