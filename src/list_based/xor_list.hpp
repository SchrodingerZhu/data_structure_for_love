#include <iostream>
#include <cstdio>

using namespace std;


class LinkedList { // A linkedlist.

public:
    struct node { // A node.
        int value; // The Key.
        node *link = reinterpret_cast<node *>(0); // The pointer prev ^ the pointer next
    };

    node *head = nullptr; // Head Pointer
    node *tail = nullptr; // Tail Pointer
    node *null = reinterpret_cast<node *>(0);

    node *combine(node *a, node *b) {  // node* ^ node*
        return reinterpret_cast<node *>(reinterpret_cast<ptrdiff_t>(a) ^ reinterpret_cast<ptrdiff_t>(b));
    }

    void add(int newValue) { // Add a node at the end.
        node *newNode = new node;
        newNode->value = newValue;
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            newNode->link = combine(tail, null);
            tail->link = combine(combine(tail->link, null), newNode);
            tail = newNode;
        }
    }

    void view() { // Print out the list.
        if (head==nullptr)
            return;
        viewList(head, null);
    }

    void viewList(node *now, node *last) {
        printf("%d ", now->value);
        if (now == tail) {
            cout << endl;
            return;
        }
        viewList(combine(now->link, last), now);
    }

    void del(int target) { // Delete a node.
        if (head==nullptr)
            return;
        deleteTarget(head, null, target);
    }

    void deleteTarget(node *now, node *last, int target) {
        if (now->value == target) {
            if (now == tail && now == head) {
                tail = nullptr;
                head = nullptr;
                return;
            }
            if (now == tail) {
                last->link = combine(combine(last->link, now), null);
                tail = last;
                return;
            }
            node *next = combine(now->link, last);
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

    bool find(int target) { // Find out if a node exists.
        if (head==nullptr)
            return false;
        return findTarget(head, null, target);
    }

    bool findTarget(node *now, node *last,int target) {
        if (now->value == target)
            return true;
        if (now == tail)
            return false;
        findTarget(combine(now->link, last), now,target);
    }

private:

};

int main() {
    int i, n;
    LinkedList listTest;
    for (i = 1; i <= 10; i++) {
        listTest.add(i);
    }
    listTest.view();
    listTest.del(10);
    listTest.view();
    cout<<listTest.find(5)<<" "<<listTest.find(10);
    return 0;
}
