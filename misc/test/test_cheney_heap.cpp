#include <iostream>
#include <array>
#include <cstddef>
#include <unordered_set>
constexpr std::size_t HEAP_SIZE = 5000000;
void *from = nullptr;
void *to = nullptr;
void *_free = nullptr;

#include <utility>
// TODO: change fields back to object* [IMPORTANT]

// TODO: FIX MORE ABOUT REMOTE_PTR

struct Object;


class heap_pointer;
std::unordered_set<Object *> roots{};
std::unordered_set<Object *> ptrs{};
template <class Child, class Parent, typename = std::enable_if_t<std::is_base_of_v<Parent, Child>>>
Child* as_pointer_of(Parent* t){
    return dynamic_cast<Child *>(t);
}



struct Object {
    std::size_t size = 0, header = 0;
    void *field = nullptr;
    heap_pointer *pointers = nullptr;
    virtual Object* _move() {throw "This should not be called"; };
};

struct SingleObjectBase: public Object{};
template <class T>
struct SingleObject: public SingleObjectBase {
    Object* _move() override {
        return move_object(this);

        //
    }
};



template<typename T>
std::size_t object_size() {
    return sizeof(Object) + sizeof(T);
};



template<typename... A>
struct Size {
    constexpr static size_t size = 0;
};
template<typename Head, typename... Tail>
struct Size<Head, Tail...> {
    constexpr static size_t size = sizeof(Object) + sizeof(typename Head::type) + Size<Tail...>::size;
};

template<typename... A>
struct Count {
    constexpr static size_t count = 0;
};
template<typename Head, typename... Tail>
struct Count<Head, Tail...> {
    constexpr static size_t count = 1 + Count<Tail...>::count;
};

template<size_t Size>
constexpr void __construct(std::array<Object *, Size> &temp, size_t position = 0) {}

template<size_t Size>
constexpr void __move(std::array<Object *, Size> &from, std::array<Object *, Size> &to, size_t position = 0) {}

class heap_pointer {
public:
    Object* object = nullptr;
    heap_pointer *next = nullptr, *prev = nullptr;
public:
    heap_pointer() = default;

    virtual void *get_real() { return nullptr; }

    explicit heap_pointer(Object *obj) {
        object = obj;
        this->next = obj->pointers;
        if (this->next) {
            this->next->prev = this;
        }
        obj->pointers = this;
    }

    heap_pointer(heap_pointer&& that) noexcept {
        this->object = that.object;
        this->next = that.next;
        this->prev = that.prev;
        if(this->prev) this->prev->next = this;
        if(this->next) this->next->prev = this;
        if(object && object->pointers == &that) {
            object->pointers = this;
        }
    }
    heap_pointer& operator=(heap_pointer&& that) noexcept {
        this->object = that.object;
        this->next = that.next;
        this->prev = that.prev;
        if(this->prev) this->prev->next = this;
        if(this->next) this->next->prev = this;
        if(object && object->pointers == &that) {
            object->pointers = this;
        }
        return *this;
    }

    friend Object;
};



template<typename T>
class remote_ptr : public heap_pointer {
public:

    remote_ptr() = default;
    explicit remote_ptr(Object *obj) : heap_pointer(obj) {

    }

    remote_ptr(remote_ptr&& that) noexcept : heap_pointer(std::forward<remote_ptr>(that))  {

    }


    remote_ptr& operator=(Object* that) noexcept {
        this->object = that;
        this->next = that->pointers;
        that->pointers = this;
        if(this->next) this->next->prev = this;
        return *this;
    }

//    remote_ptr& operator=(remote_ptr&& that) noexcept {
//        this->field = that.field;
//        this->next = that.next;
//        this->prev = that.prev;
//        if(this->prev) this->prev->next = this;
//        if(this->next) this->next->prev = this;
//    }

    T& operator*() {
        if(std::is_base_of_v<Object, T>) {
            auto x = reinterpret_cast<T *>(this->object);
            return *x;
        } else {
            auto x = reinterpret_cast<T *>(this->object->field);
            return *x;
        }
    }

    T* operator->() {
        return &operator*();
    }

};



template<typename T>
struct Ptr {
    using base_type = T*;
    using type = remote_ptr<T>;
    constexpr static bool is_ptr = true;
    using is_ptr_type = std::true_type;
};
template<typename T>
struct Local {
    using type = T;
    constexpr static bool is_ptr = false;
    using is_ptr_type = std::false_type;
};

template <size_t N, typename T, typename... Tn>
struct TypeHelper{
    using type = typename TypeHelper<N - 1, Tn...>::type;
};

template <typename T, typename... Tn>
struct TypeHelper<0, T, Tn...>{
    using type = T;
};



template<typename T, typename ...Args>
SingleObject<T> *make_object(Args&& ...args) {
    auto ptr = reinterpret_cast<SingleObject<T> *>(_free);
    ::new(ptr) SingleObject<T>;
    ptr->size = object_size<T>();
    ptr->header = sizeof(Object);
    ptr->field = reinterpret_cast<char *>(_free) + sizeof(Object);
    ::new (reinterpret_cast<T *>(ptr->field)) T (std::forward<Args>(args)...);
    _free = reinterpret_cast<char *>(_free) + object_size<T>();
    return ptr;
}

template<typename T>
SingleObject<T> *move_object(SingleObject<T>* from) { /// @attention: must move following the order
    auto to = reinterpret_cast<SingleObject<T> *>(_free);
    ::new(to) SingleObject<T>;
    to->size = from->size;
    to->header = from->header;
    to->field = reinterpret_cast<char *>(_free) + sizeof(Object);
    to->pointers = from->pointers;
    auto t = from->pointers;
    while(t) {
        t->object = to;
        t= t->next;
    }
    ::new (reinterpret_cast<T *>(to->field)) T (std::move(*reinterpret_cast<T *>(from->field)));
    to->pointers = from->pointers;
    _free = reinterpret_cast<char *>(_free) + object_size<T>();
    return to;
}


template <class T>
struct wrapper_ptr{
    T* object;
    wrapper_ptr(T *t) {
        object = t;
    }
};



struct CollectableBase: public Object {};

template<typename T, typename ...Tn>
struct Collectable : public CollectableBase {
    std::array<Object*, Count<T, Tn...>::count> fields; //no init please
    Collectable() = default;
    template <size_t N>
    auto& get(){
        //return _get<N>(typename TypeHelper<N, T, Tn...>::type::is_ptr_type());
        return *reinterpret_cast<typename TypeHelper<N, T, Tn...>::type::type*>(fields[N]->field);
    }

    template<typename U, typename ...Args>
    static void construct_self_at(remote_ptr<U> &address, Args &&... args);


    Object* _move() override {
        auto res = move_collectable(this);
        return res;
    }
    template <class U, typename ...Args>
    static remote_ptr<U> generate(Args&& ...);

private:
    //Collectable() {;}
//    template <size_t N>
//    auto& _get(std::true_type){
//        return *reinterpret_cast<typename TypeHelper<N, T, Tn...>::type::type*>(pointers[N]);
//    }
//
//    template <size_t N>
//    auto& _get(std::false_type){
//        return *reinterpret_cast<typename TypeHelper<N, T, Tn...>::type::type*>(pointers[N]->object->field);
//    }


    template<typename U, typename ...Un>
    friend Collectable<U, Un...>* make_collectable(std::size_t);
};





template<class T, class ...Tn,  size_t Size>
constexpr void __construct(std::array<Object *, Size> &temp, size_t position = 0) {

    temp[position] = make_object<typename T::type>();
    if(T::is_ptr) {
        ptrs.insert(temp[position]);
    }

    __construct<Tn...>(temp, position + 1);
}


template<class T, class ...Tn,  size_t Size>
constexpr void __move(std::array<Object *, Size> &from, std::array<Object *, Size> &to, size_t position = 0) {

    to[position] = move_object<typename T::type>(
                reinterpret_cast<SingleObject<typename T::type>*>(from[position]));
    if(ptrs.count(from[position])) {
        ptrs.erase(from[position]);
        ptrs.insert(to[position]);
    }
    __move<Tn...>(from, to, position + 1);
}


template<typename T, typename ...Tn>
std::size_t collectable_size() {
    return sizeof(Collectable<T, Tn...>) + Size<T, Tn...>::size;
};



template<typename U, typename T, typename ...Tn, typename ...Args>
Collectable<T, Tn...>* make_collectable(Args... args) {
    auto ptr = reinterpret_cast<Collectable<T, Tn...> *>(_free);
    std::array<Object*, Count<T, Tn...>::count> temp{};
    ::new (ptr) Collectable<T, Tn...>();
    auto field = ptr->field = _free = reinterpret_cast<char *>(_free) + sizeof(Collectable<T, Tn...>);
    __construct<T, Tn...>(ptr->fields);
    for(size_t i = 0; i < Count<T, Tn...>::count; ++i) {
        temp[i] = ptr->fields[i];
    }
    ::new(ptr) U(std::forward<Args>(args)...);
    for(size_t i = 0; i < Count<T, Tn...>::count; ++i) {
        ptr->fields[i] = temp[i];
    }
    ptr->header = sizeof(U);
    ptr->size = collectable_size<T, Tn...>();
    ptr->field = field;
    return ptr;
}


template<typename T, typename ...Tn>
Collectable<T, Tn...>* move_collectable(Collectable<T, Tn...>* from) {
    std::array<Object*, Count<T, Tn...>::count> temp{};
    auto to = reinterpret_cast<decltype(from)>(_free);
    ::new (to) Collectable<T, Tn...>();
    auto field = to->field = _free = reinterpret_cast<char *>(_free) + sizeof(Collectable<T, Tn...>);
    __move<T, Tn...>(from->fields, to->fields);
    auto t = from->pointers;
    while(t) {
        t->object = to;
        t= t->next;
    }
    to->pointers = from->pointers;
    to->size = from->size;
    to->header = from->header;
    to->field = field;
    return to;
}

template<typename T, typename ...Args>
void construct_at(remote_ptr<T>& address, Args&& ...args) {
    address = make_object<T>(std::forward<Args>(args)...);
};


template <typename T, typename ...Tn>
template <typename U, typename ...Args>
void Collectable<T, Tn...>::construct_self_at(remote_ptr<U> &address, Args &&... args) {
    auto object = reinterpret_cast<U *>(make_collectable<U, T, Tn...>(std::forward<Args>(args)...));
    address = reinterpret_cast<Object *>(object);
}




template<typename T, typename... Tn>
template<class U, typename ...Args>
remote_ptr<U> Collectable<T, Tn...>::generate(Args&& ...args) {
    auto ptr = make_collectable<U, T, Tn...>(std::forward<Args>(args)...);
    roots.insert(ptr);
    return remote_ptr<U>(reinterpret_cast<U *>(ptr));
}


class Node: public Collectable<
        Local<int>, Local<int>, Local<size_t>, Ptr<size_t>, Local<std::string>, Ptr<Node>>
{
    public:
        Node() = default;

        Node(int t, int child = 0) {
            get<1>() = 1234567890 + 1;
            construct_at(get<3>(), 1234);
            construct_self_at(get<5>());
        }
};

void garbage_collect() {
    auto scan = _free = to;
    decltype(roots) new_roots{};
    for (Object* i: roots) {
        new_roots.insert(i->_move());
    }
    roots = std::move(new_roots);
    while(scan != _free) {
        auto t = reinterpret_cast<Object *>(scan);
        auto p = as_pointer_of<CollectableBase>(t);
        if(p) {
            auto objects = reinterpret_cast<Object *>(p->field);
            while(reinterpret_cast<char *>(objects) - reinterpret_cast<char *>(p) != p->size) {
                if(ptrs.count(objects)) {
                    auto ptr = reinterpret_cast<heap_pointer *>(objects->field);
                    if(ptr->object) {
                        ptr->object->_move();
                    }
                }
                objects = reinterpret_cast<Object *>(reinterpret_cast<char *>(objects) + objects->size);
            }
        }
        scan = reinterpret_cast<char *>(scan) + t->size;
    }
    std::swap(from, to);
}

//struct A{
//    virtual void meow() {
//        std::cout << "A" << std::endl;
//    }
//};
//
//struct B: public A{
//    void meow() override {
//        std::cout << "B" << std::endl;
//    }
//};
//
//struct C: public A{
//    void meow() override {
//        std::cout << "C" << std::endl;
//    }
//};
void wander(Object *t) {
    if(t) {
        std::cout << "------------------------------\n";
        std::cout << "address: " << t << std::endl;
        std::cout << "size: " << t->size << std::endl;
        int i = 0; auto p = t->pointers;
        while(p) {
            i++;
            p = p->next;
        }
        std::cout << "pointers num: " << i << std::endl;
        std::cout << "pointers:";
        p = t->pointers;
        while(p) {
            std::cout << p << " ";
            p = p->next;
        }
        std::cout << std::endl;
        std::cout << "field: " << t->field << std::endl;
        std::cout << "header: " << t->header << std::endl;

            //std::cout << "This is a collectable!" << std::endl;
        if(ptrs.count(t)) {
            auto m = reinterpret_cast<heap_pointer *>(t->field);
            std::cout << "[pointer]"  << std::endl;
            std::cout << "to: " << m->object << std::endl;
            if(m->object) wander(m->object);
        }
        if(as_pointer_of<CollectableBase>(t)) {
            std::cout << "[collectable]"  << std::endl;
            auto field = reinterpret_cast<char *>(t->field);
            while (field  != t->size + reinterpret_cast<char *>(t)) {
                wander(reinterpret_cast<Object *>(field));
                field += reinterpret_cast<Object *>(field)->size;
            }
        }
    }
}
int main() {
    from = ::operator new(HEAP_SIZE * 2);
    to = reinterpret_cast<char *>(from) + HEAP_SIZE;
    _free = from;
//    auto t = Node::generate<Node>(1);
    auto t = Node::generate<Node>(2);
    auto h = Node::generate<Node>(2);
    wander(t.object);
    garbage_collect();
    std::cout << std::endl;
    wander(t.object);
    garbage_collect();
    std::cout << std::endl;
    wander(t.object);
    garbage_collect();
    garbage_collect();
    garbage_collect();
    garbage_collect();
    garbage_collect();
    garbage_collect();
    garbage_collect();
    garbage_collect();
    garbage_collect();
    garbage_collect();
    garbage_collect();
    garbage_collect();
    std::cout << std::endl;
    wander(t.object);

    // t->test();
    //garbage_collect();
    // std::cout << sizeof(Node) << std::endl << sizeof(Collectable<Ptr<Node>, Local<int>, Ptr<int>>);
    return 0;
}