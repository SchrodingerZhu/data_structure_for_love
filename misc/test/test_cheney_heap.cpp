#include <iostream>
#include <array>
#include <cstddef>
#include <unordered_set>
#include <cheney_heap.hpp>
using namespace data_structure;
class Node: public CheneyHeap::Collectable<
        Local<int>,                        // 0
        Local<int>,                        // 1
        Local<size_t>,                     // 2
        Ptr<size_t>,                       // 3
        Ptr<std::string>,                  // 4
        Ptr<Node>>                         // 5
{
    public:
        Node() = default;
        void test() {
            std::cout << get<5>()->get<0>() << std::endl;
            std::cout << *get<4>() << std::endl;
        }
        explicit Node(int t = 0) {
            get<0>() = 114514;
            construct_at(get<3>(), 1234);
            construct_at(get<4>(), "1234");
            if(t)
                construct_self_at(get<5>(), t - 1);
        }
};
int main() {
    using namespace data_structure;
    auto test = data_structure::CheneyHeap();
    auto test2 = data_structure::CheneyHeap::Collectable<Local<int>>();
    //test2.move_children(nullptr);
    //from = ::operator new(HEAP_SIZE * 2);

    auto t = Node::generate<Node>(test, 2);
    test.wander();
    test.garbage_collect();
    //auto h = Node::generate<Node>(2);
    std::cout << std::endl;
    //test.wander();
    t->test();
    // t->test();
    //garbage_collect();
    // std::cout << sizeof(Node) << std::endl << sizeof(Collectable<Ptr<Node>, Local<int>, Ptr<int>>);
    return 0;
}