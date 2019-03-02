//
// Created by schrodinger on 2/22/19.
//
// TODO: Change children to hash multiset to handle one-father-many-pointers situation
#ifndef DATA_STRUCTURE_FOR_LOVE_CHENEY_HEAP_HPP
#define DATA_STRUCTURE_FOR_LOVE_CHENEY_HEAP_HPP

#include <cstddef>
#include <vector>
#include <unordered_set>

#include <sundries.hpp>
#include <object_management.hpp>
#include <memory>
#include <cassert>

#ifdef DEBUG
#include <iostream>
#endif // DEBUG


/// @attention when using this heap, it is encouraged not to use recursive constructors, because multi-grow may fail to
// detect real space. However, if your heap size if big enough, it is okay to use that.
namespace data_structure {
    class CheneyHeap{
        struct heap_pointer;
        struct Object;
        class SingleObjectBase;
        class CollectableBase;
        char *free = nullptr, *from = nullptr, *to = nullptr;
        size_t heap_size {};
        std::unordered_set<Object *> active {};
        std::unordered_set<Object *> copied {};
    public:
        template <typename T> class SingleObject;
        template <typename T, typename... Tn> class Collectable;
        template <typename T> class remote_ptr;
        template <typename T> struct Local;
        template <typename T> struct Ptr;
        std::unordered_set<Object *> roots {};
        explicit CheneyHeap(size_t heap_size = 20000) {
            this->heap_size = heap_size;
            from = free = reinterpret_cast<char*>(::operator new(heap_size << 1));
            to = from + heap_size;
        }
        ~CheneyHeap() {
            if(from < to) delete from;
            else delete to;
        }
    private:

        template <typename T, typename... Tn>
        Collectable<T, Tn...>* move_collectable(Collectable<T, Tn...>* from);
        template<typename T, typename ...Args> SingleObject<T> *make_object(Args&& ...args);
        template<typename T> constexpr static std::size_t object_size();
        template<typename T, typename ...Tn> constexpr static std::size_t collectable_size();
        template<typename T> SingleObject<T> *move_object(SingleObject<T>* from);

        template<size_t Size>
        constexpr void __construct(std::array<void *, Size> &temp, size_t position = 0) {}

        template<class T, class ...Tn,  size_t Size>
        constexpr void __construct(std::array<void *, Size> &temp, size_t position = 0);

        template<size_t Size>
        constexpr void __move(std::array<void *, Size> &from, std::array<void *, Size> &to, size_t position = 0) {}

        template<class T, class ...Tn,  size_t Size>
        constexpr void __move(std::array<void *, Size> &from, std::array<void *, Size> &to, size_t position = 0);

        template<typename U, typename T, typename ...Tn, typename ...Args>
        Collectable<T, Tn...>* make_collectable(Args... args);

        template<typename T, typename ...Args>
        void construct_at(remote_ptr<T>& address, Args&& ...args);

        bool in_new_range(char const *position) {
            return position >= to && position - to < heap_size;
        }

        void wander(Object *t);
    public:
        void wander() {
            for(auto i: roots) {
                wander(i);
            }
        }
        void garbage_collect();
        void grow(size_t least) {
            while(heap_size <= least) heap_size <<= 3;
            auto temp = from < to ? from : to;
            to = reinterpret_cast<char*>(::operator new(heap_size << 1));
            garbage_collect();
            to = from + heap_size;
            delete temp;
        }
    };


    template<class T, class ...Tn,  size_t Size>
    constexpr void CheneyHeap::__construct(std::array<void *, Size> &temp, size_t position) {
        temp[position] = new (free) typename T::type ();
        free = free + sizeof(typename T::type);
        __construct<Tn...>(temp, position + 1);
    }

    template<class T, class ...Tn,  size_t Size>
    constexpr void CheneyHeap::__move(std::array<void *, Size> &from, std::array<void *, Size> &to, size_t position) {
        to[position] = ::new (free) typename T::type(std::move(
                *reinterpret_cast<typename T::type*>(from[position])));
        free = free + sizeof(typename T::type);
        __move<Tn...>(from, to, position + 1);
    }

    struct CheneyHeap::Object{
        std::size_t size = 0, header = 0;
        void *field = nullptr;
        CheneyHeap* heap; // no initialization
        heap_pointer *pointers = nullptr;
        virtual Object* _move() {throw "This should not be called"; };
        virtual ~Object()  = default;
    };



    class CheneyHeap::SingleObjectBase: public Object {
        virtual void self_destroy() {throw "This should not be called.\n";}
        friend CheneyHeap;
    };

    struct CheneyHeap::CollectableBase: public Object {
        virtual void move_children() {throw "This should not be called.\n";}
        virtual void self_destroy() {throw "This should not be called.\n";}
        virtual void show_children() {throw "This should not be called.\n";}
        friend CheneyHeap;
    };


    template <typename T>
    class CheneyHeap::SingleObject: public SingleObjectBase{
        Object* _move() override {
            return heap->move_object(this);
        }
        ~SingleObject() override {
            utils::destroy_at(reinterpret_cast<T *>(field));
        }
    };

    template <typename T>
    struct CheneyHeap::Local {
        using type = T;
        using base_type = T;
        constexpr static bool is_ptr = false;
        using is_ptr_type = std::false_type;
    };

    template<typename T>
    struct CheneyHeap::Ptr {
        using base_type = T;
        using type = CheneyHeap::remote_ptr<T>;
        constexpr static bool is_ptr = true;
        using is_ptr_type = std::true_type;
    };

    template <typename T>
    using Ptr = CheneyHeap::Ptr<T>;

    template <typename T>
    using Local = CheneyHeap::Local<T>;

    template<typename T, typename ...Args> CheneyHeap::SingleObject<T> *CheneyHeap::make_object(Args&& ...args) {
        if(free + object_size<T>() - from > heap_size) {
            garbage_collect();
        }
        if(free + object_size<T>() - from > heap_size) {
            grow(free + object_size<T>() - from);
        }
        auto ptr = reinterpret_cast<SingleObject<T> *>(free);
        active.insert(ptr);
        ::new(ptr) SingleObject<T>;
        ptr->size = object_size<T>();
        ptr->heap = this;
        ptr->header = sizeof(Object);
        ptr->field = free + sizeof(Object);
        ::new (reinterpret_cast<T *>(ptr->field)) T (std::forward<Args>(args)...);
        free = free + object_size<T>();
        return ptr;
    }

    template<typename T>
    CheneyHeap::SingleObject<T> *CheneyHeap::move_object(SingleObject<T>* from) { /// @attention: must move following the order
        auto to = reinterpret_cast<SingleObject<T> *>(free);
        copied.insert(to);
        ::new(to) SingleObject<T>;
        to->heap = this;
        to->size = from->size;
        to->header = from->header;
        to->field = free + sizeof(Object);
        to->pointers = from->pointers;
        auto t = from->pointers;
        while(t) {
            t->object = to;
            t= t->next;
        }
        ::new (reinterpret_cast<T *>(to->field)) T (std::move(*reinterpret_cast<T *>(from->field)));
        to->pointers = from->pointers;
        free = free + object_size<T>();
        return to;
    }




    template<typename T, typename... Tn>
    CheneyHeap::Collectable<T, Tn...> *
    CheneyHeap::move_collectable(CheneyHeap::Collectable<T, Tn...> *from) {
        std::array<void*, utils::Count<T, Tn...>::count> temp{};
        auto to = reinterpret_cast<decltype(from)>(free);
        copied.insert(to);
        ::new (to) Collectable<T, Tn...>();
        auto field = to->field = free = free + sizeof(Collectable<T, Tn...>);
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
        to->heap = this;
        return to;
    }

    struct CheneyHeap::heap_pointer {
        Object* object = nullptr;
        heap_pointer *next = nullptr, *prev = nullptr;
        heap_pointer() = default;

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

    template <typename T>
    class CheneyHeap::remote_ptr: public heap_pointer {
    public:

        remote_ptr() = default;
        explicit remote_ptr(Object *obj) : heap_pointer(obj) {

        }

        remote_ptr(remote_ptr&& that) noexcept : heap_pointer(std::forward<remote_ptr>(that))  {}

        remote_ptr(remote_ptr const & that) {
            this->object = that.object;
            this->next = that.next;
            auto& m = const_cast<remote_ptr &>(that);
            this->prev = &m;
            m.next = this;
            if(this->next) this->next->prev = this;
        }

        remote_ptr& operator=(remote_ptr const& that) {
            this->object = that.object;
            this->next = that.next;
            auto& m = const_cast<remote_ptr &>(that);
            this->prev = &m;
            m.next = this;
            if(this->next) this->next->prev = this;
            return *this;
        }

        remote_ptr& operator=(Object* that) noexcept {
            this->object = that;
            this->next = that->pointers;
            that->pointers = this;
            if(this->next) this->next->prev = this;
            return *this;
        }

        remote_ptr& operator=(nullptr_t) noexcept {
            if (this->object && this->object->pointers == this) {
                this->object->pointers = this->next;
                if(this->object->pointers == nullptr && this->object->heap->roots.count(this->object)) {
                    utils::destroy_at(this->object);
                    this->object->heap->roots.erase(this->object);
                    this->object->heap->active.erase(this->object);
                }
            }
            this->object = nullptr;
            if(this->next) this->next->prev = this->prev;
            if(this->prev) this->prev->next = this->next;
            this->next = this->prev = nullptr;
            return *this;
        }

        bool operator==(nullptr_t) noexcept {
            return this->object;
        }

        ~remote_ptr() {
            if (this->object && this->object->pointers == this) {
                this->object->pointers = this->next;
                if(this->object->pointers == nullptr && this->object->heap->roots.count(this->object)) {
                    utils::destroy_at(this->object);
                    this->object->heap->roots.erase(this->object);
                    this->object->heap->active.erase(this->object);
                }
            }
            this->object = nullptr;
            if(this->next) this->next->prev = this->prev;
            if(this->prev) this->prev->next = this->next;
            this->next = this->prev = nullptr;
        }

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

    template<typename T> constexpr std::size_t CheneyHeap::object_size() {
        return sizeof(Object) + sizeof(T);
    }

    template<typename T, typename ...Tn>
    constexpr std::size_t CheneyHeap::collectable_size() {
        return sizeof(Collectable<T, Tn...>) + utils::Size<T, Tn...>::size;
    };

    template<typename U, typename T, typename ...Tn, typename ...Args>
    CheneyHeap::Collectable<T, Tn...>* CheneyHeap::make_collectable(Args... args) {
        auto p = free + collectable_size<T, Tn...>() - from;
        if(free + collectable_size<T, Tn...>() - from > heap_size) {
            garbage_collect();
        }
        if(free + collectable_size<T, Tn...>() - from > heap_size) {
            grow(free + collectable_size<T, Tn...>() - from);
        }
        auto ptr = reinterpret_cast<Collectable<T, Tn...> *>(free);
        active.insert(ptr);
        std::array<void *, utils::Count<T, Tn...>::count> temp{};
        ::new (ptr) Collectable<T, Tn...>();
        ptr->heap = this;
        auto field = ptr->field = free = free + sizeof(Collectable<T, Tn...>);
        __construct<T, Tn...>(ptr->fields);
        for(size_t i = 0; i < utils::Count<T, Tn...>::count; ++i) {
            temp[i] = ptr->fields[i];
        }
        ::new(ptr) U(std::forward<Args>(args)...);
        for(size_t i = 0; i < utils::Count<T, Tn...>::count; ++i) {
            ptr->fields[i] = temp[i];
        }

        ptr->header = sizeof(U);
        ptr->size = collectable_size<T, Tn...>();
        ptr->field = field;
        ptr->heap = this;
        return ptr;
    }

    template <typename T, typename ...Tn>
    class CheneyHeap::Collectable: private CollectableBase {
        std::array<void *, utils::Count<T, Tn...>::count> fields;
        Object* _move() override {
            return heap->move_collectable(this);
        }
        friend CheneyHeap;

    private:
        void move_children() override {
            _move_children<0, T, Tn...>();
        }

        template <std::size_t N, typename U, typename ...Un>
        void _move_children() {
            if(U::is_ptr) {
                auto c_ptr = reinterpret_cast<heap_pointer *>(fields[N]);
                if(c_ptr->object && !heap->in_new_range(reinterpret_cast<char *>(c_ptr->object))) {
                    c_ptr->object->_move();
                }
            }
            //if(N == 0) return;
            _move_children<N + 1, Un...>();
        }

        template  <std::size_t>
        void _move_children(){; }


        template <std::size_t N, typename U, typename ...Un>
        void _self_destroy() {
            auto c_ptr = reinterpret_cast<typename U::type *>(fields[N]);
            utils::destroy_at(c_ptr);
            _self_destroy<N + 1, Un...>();
        }

        template  <std::size_t>
        void _self_destroy(){; }

        void show_children() override {
            _show_children<0, T, Tn...>();
        }

        template <std::size_t N, typename U, typename ...Un>
        void _show_children() {
            std::cout << "------------------------------" << std::endl;
            std::cout << "address: " << fields[N] << std::endl;
            std::cout << "size: " << sizeof(typename U::type) << std::endl;
            if(U::is_ptr) {
                auto c_ptr = reinterpret_cast<heap_pointer *>(fields[N]);
                std::cout << "to: " << c_ptr->object << std::endl;
                std::cout << "[pointer]" << std::endl;
                if(c_ptr->object) {
                    heap->wander(c_ptr->object);
                }

            } else {
                std::cout << "[basic chunk]" << std::endl;
                //std::cout << std::endl;
            }
            //if(N == 0) return;
            _show_children<N + 1, Un...>();
        }
        template  <std::size_t>
        void _show_children(){; }


    public:
        Collectable() = default;
        ~Collectable() override {
            _self_destroy<0, T, Tn...>();
        }
        template <size_t N>
        auto& get() {
            return *reinterpret_cast<typename utils::TypeHelper<N, T, Tn...>::type::type*>(fields[N]);
        }

        template <typename U, typename ...Args>
        void construct_self_at(remote_ptr<U> &address, Args&&... args);

        template<typename U, typename... Args>
        void construct_at(remote_ptr<U> &address, Args &&... args);

        template<class U, typename ...Args>
        static remote_ptr<U> generate(CheneyHeap & heap, Args&& ...args);
    };

    template<typename T, typename... Args>
    void CheneyHeap::construct_at(CheneyHeap::remote_ptr<T> &address, Args &&... args) {
        address = make_object<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Tn>
    template<typename U, typename... Args>
    void CheneyHeap::Collectable<T, Tn...>::construct_self_at(remote_ptr<U> &address, Args &&... args) {
        auto object = reinterpret_cast<U *>(heap->make_collectable<U, T, Tn...>(std::forward<Args>(args)...));
        address = reinterpret_cast<Object *>(object);
    }

    template<typename T, typename... Tn>
    template<class U, typename... Args>
    CheneyHeap::remote_ptr<U> CheneyHeap::Collectable<T, Tn...>::generate(CheneyHeap & heap, Args &&... args) {
        auto ptr = heap.make_collectable<U, T, Tn...>(std::forward<Args>(args)...);
        heap.roots.insert(ptr);
        return remote_ptr<U>(reinterpret_cast<U *>(ptr));
    }

    void CheneyHeap::garbage_collect() {
        copied.clear();
        auto scan = free = to;
        decltype(roots) new_roots{};
        for (Object* i: roots) {
            auto ptr = i->_move();
            new_roots.insert(ptr);
        }
        roots = std::move(new_roots);
        while(scan != free) {
            auto t = reinterpret_cast<Object *>(scan);
            auto p = utils::as_pointer_of<CollectableBase>(t);
            if(p) p->move_children();
            scan = scan + t->size;
        }
        std::swap(from, to);
        for(auto i: active) {
            utils::destroy_at(i);
        }
        active = copied;
    }

    void CheneyHeap::wander(CheneyHeap::Object *t) {
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
//        if(ptrs.count(t)) {
//            auto m = reinterpret_cast<heap_pointer *>(t->field);
//            std::cout << "[pointer]"  << std::endl;
//            std::cout << "to: " << m->object << std::endl;
//            if(m->object) wander(m->object);
//        }
                if(utils::as_pointer_of<CollectableBase>(t)) {
                    std::cout << "[collectable]"  << std::endl;
                    reinterpret_cast<CollectableBase *>(t)->show_children();
                } else {
                    std::cout << "[single object]"  << std::endl;
                    std::cout << "------------------------------" << std::endl;
                    std::cout << "address: " << t->field << std::endl;
                    std::cout << "size: " << t->size - t->header << std::endl;
                    std::cout << "[basic chunk]"  << std::endl;
                }
            }


    }

    template <typename T, typename ...Tn>
    template<typename U, typename... Args>
    void CheneyHeap::Collectable<T, Tn...>::construct_at(CheneyHeap::remote_ptr<U> &address, Args &&... args) {
        heap->construct_at(address, std::forward<Args>(args)...);
    }

};

#endif //DATA_STRUCTURE_FOR_LOVE_CHENEY_HEAP_HPP
