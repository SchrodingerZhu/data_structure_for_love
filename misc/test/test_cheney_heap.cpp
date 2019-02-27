#include <iostream>
#include <array>
#include <cstddef>
#include <unordered_set>
constexpr std::size_t HEAP_SIZE = 500000;
void *from = nullptr;
void *to = nullptr;
void *_free = nullptr;

#include <utility>


struct Object;


class heap_pointer;

template <class Child, class Parent, typename = std::enable_if_t<std::is_base_of_v<Parent, Child>>>
bool is_pointer_of(Parent* t){
    return dynamic_cast<Child *>(t) != nullptr;
}

struct Object {
    std::size_t size;
    void *field;
    heap_pointer *pointers;
};

std::unordered_set<Object *> roots{};
std::unordered_set<Object *> pointers{};
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
constexpr void __construct(std::array<heap_pointer *, Size> &temp, size_t position = 0) {}

template<size_t Size>
constexpr void __move(std::array<heap_pointer *, Size> &from, std::array<heap_pointer *, Size> &to, size_t position = 0) {}

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

    remote_ptr(remote_ptr&& that) noexcept : heap_pointer(that)  {

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

struct CollectableBase: public Object{
    size_t header;
};

template<typename T, typename ...Args>
Object *make_object(Args&& ...args) {
    auto ptr = reinterpret_cast<Object *>(_free);
    ::new(ptr) Object;
    ptr->size = object_size<T>();
    ptr->field = reinterpret_cast<char *>(_free) + sizeof(Object);
    ::new (reinterpret_cast<T *>(ptr->field)) T (std::forward<Args>(args)...);
    _free = reinterpret_cast<char *>(_free) + object_size<T>();
    return ptr;
}

template<typename T>
Object *move_object(Object* from) { /// @attention: must move following the order
    auto to = reinterpret_cast<Object *>(_free);
    ::new(to) Object;
    to->size = from->size;
    to->field = reinterpret_cast<char *>(_free) + sizeof(Object);
    ::new (reinterpret_cast<T *>(to->field)) T (std::move(*reinterpret_cast<T *>(from->field)));
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


template<typename T, typename ...Tn>
struct Collectable : public CollectableBase {
    std::array<heap_pointer*, Count<T, Tn...>::count> pointers;

    template <size_t N>
    auto& get(){
        //return _get<N>(typename TypeHelper<N, T, Tn...>::type::is_ptr_type());
        return *reinterpret_cast<typename TypeHelper<N, T, Tn...>::type::type*>(pointers[N]->object->field);
    }

    template<typename U, typename ...Args>
    static void construct_self_at(remote_ptr<U> &address, Args &&... args);

    template <class U, typename ...Args>
    static wrapper_ptr<U> generate(Args&& ...);

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
constexpr void __construct(std::array<heap_pointer *, Size> &temp, size_t position = 0) {
    if (T::is_ptr) {
        pointers.insert(reinterpret_cast<Object *>(free));
        temp[position] = new heap_pointer(make_object<typename T::type>());
    } else {
        temp[position] = new heap_pointer(make_object<typename T::type>());
    }

    __construct<Tn...>(temp, position + 1);
}


template<class T, class ...Tn,  size_t Size>
constexpr void __move(std::array<heap_pointer *, Size> &from, std::array<heap_pointer *, Size> &to, size_t position = 0) {
    if (T::is_ptr) {
        pointers.insert(reinterpret_cast<Object *>(_free));
        to[position] = new heap_pointer(move_object<typename T::type>(from[position]->object));
        delete from[position];
    } else {
        to[position] = new heap_pointer(move_object<typename T::type>(from[position]->object));
    }

    __construct<Tn...>(from, to, position + 1);
}


template<typename T, typename ...Tn>
std::size_t collectable_size() {
    return sizeof(Collectable<T, Tn...>) + Size<T, Tn...>::size;
};



template<typename U, typename T, typename ...Tn, typename ...Args>
Collectable<T, Tn...>* make_collectable(Args... args) {
    auto ptr = reinterpret_cast<Collectable<T, Tn...> *>(_free);
    std::array<heap_pointer*, Count<T, Tn...>::count> temp{};
    ::new (ptr) Collectable<T, Tn...>();
    auto field = ptr->field = _free = reinterpret_cast<char *>(_free) + sizeof(Collectable<T, Tn...>);
    __construct<T, Tn...>(ptr->pointers);
    for(size_t i = 0; i < Count<T, Tn...>::count; ++i) {
        temp[i] = ptr->pointers[i];
    }
    ::new(ptr) U(std::forward<Args>(args)...);
    for(size_t i = 0; i < Count<T, Tn...>::count; ++i) {
        ptr->pointers[i] = temp[i];
    }
    ptr->size = sizeof(U);
    ptr->field = field;
    return ptr;
}


template<typename U, typename T, typename ...Tn>
Collectable<T, Tn...>* move_collectable(Collectable<T, Tn...>* from) {
    std::array<heap_pointer*, Count<T, Tn...>::count> temp{};
    auto to = reinterpret_cast<decltype(from)>(_free);
    ::new (to) Collectable<T, Tn...>();
    auto field = to->field = _free = reinterpret_cast<char *>(_free) + sizeof(Collectable<T, Tn...>);
    __move<T, Tn...>(from->pointers, to->pointers);
    for(size_t i = 0; i < Count<T, Tn...>::count; ++i) {
        temp[i] = to->pointers[i];
    }
    ::new(to) U(std::move(*reinterpret_cast<U *>(from)));
    for(size_t i = 0; i < Count<T, Tn...>::count; ++i) {
        to->pointers[i] = temp[i];
    }
    to->size = sizeof(U);
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
    address = reinterpret_cast<Object *>(make_collectable<U, T, Tn...>(std::forward<Args>(args)...));
}




template<typename T, typename... Tn>
template<class U, typename ...Args>
wrapper_ptr<U> Collectable<T, Tn...>::generate(Args&& ...args) {
    auto ptr = make_collectable<U, T, Tn...>(std::forward<Args>(args)...);
    return wrapper_ptr<U>(reinterpret_cast<U *>(ptr));
}


class Node: public Collectable<
        Ptr<Node>,
        Local<int>,
        Ptr<int>,
        Local<std::string>,
        Ptr<std::string>,
        Local<std::array<int, 10000>>>
{
        int i = 123;
    public:
        void test(){
            auto& x = this->get<0>();
            construct_self_at(x);
            x->get<1>() = 1;
            x->get<3>() = "1235";
            construct_at(this->get<2>(), 123);
            construct_at(this->get<4>(), "ZYF");
            construct_self_at(x->get<0>(), 1);
            //x->get<0>()->get<1>() = 114514;
            std::cout << x->get<0>()->get<1>() << std::endl;
            std::cout << x->get<3>() << std::endl;
            std::cout << *this->get<2>() << std::endl;
            std::cout << *this->get<4>() << std::endl;
            for(int i = 0; i < 10; ++i) {
                get<5>()[i] = i;
            }
            for(int i = 0; i < 10; ++i) {
                std::cout << get<5>()[i] << std::endl;
            }

        }
        Node() = default;
        Node(int t) {
            get<1>() = 1234567890 + 1;
            i = 1000;
        }
};




int main() {
    from = ::operator new(HEAP_SIZE * 2);
    to = reinterpret_cast<char *>(from) + HEAP_SIZE;
    _free = from;
    auto t = Node::generate<Node>(1);
    t.object->test();
    std::cout << sizeof(Node) << std::endl << sizeof(Collectable<Ptr<Node>, Local<int>, Ptr<int>>);
    return 0;
}