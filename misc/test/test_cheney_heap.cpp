//
// Created by schrodinger on 2/22/19.
//
#define DEBUG
#include <cheney_heap.hpp>
#include <sundries.hpp>

int main() {
//    int* a = new int, *b = new int;
    data_structure::CheneyHeap<int> haha;
    auto obj1 = haha.get(); *obj1 = 1;
    auto obj2 = haha.get(); *obj2 = 2;
    auto obj3 = haha.get(obj1); *obj3 = 3;
    auto obj4 = haha.get(obj1); *obj4 = 4;
    auto obj5 = haha.get(obj1); *obj5 = 5;
    auto obj6 = haha.get(obj2); *obj6 = 6;
    auto obj7 = haha.get(obj4); *obj7 = 7;
    data_structure::cheney_ptr<int> obj72 = obj7;
    data_structure::cheney_ptr<int> obj73(obj7, obj1);
    data_structure::cheney_ptr<int> obj74(std::move(obj72));
    data_structure::CheneyHeap<std::string> haha2;
    auto obj8 = haha2.get(); *obj8 = "213";
    auto obj9 = haha2.get(); *obj9 = "2313";
    auto obj10 = haha2.get(obj8); *obj10 = "1234567";
    haha2.garbage_collect();
    haha2.debug();
    haha2.clear_visits();
    std::cout << "\n\n\n Another: \n";
    haha2.debug(obj9);
    return 0;
}

#undef DEBUG