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

#include <memory>
#ifdef DEBUG
#include <iostream>
#endif // DEBUG

namespace data_structure {
    class CheneyHeap{
        struct heap_pointer;
        struct Object;
        class SingleObjectBase;
        class CollectableBase;
        char *free = nullptr, *from = nullptr, *to = nullptr;
    public:
        template <typename T> class SingleObject;
        template <typename T, typename... Tn> class Collectable;
        template <typename T> class remote_ptr;
        template <typename T> struct Local;
        template <typename T> struct Ptr;
    private:
        template <typename T>
        static SingleObject<T>* move_object(CheneyHeap::SingleObject<T> *address, char*& to);
        template <typename T, typename... Tn>
        static Collectable<T, Tn...>* move_collectable(Collectable<T, Tn...>* address, char*& to);
    };

    struct CheneyHeap::heap_pointer {
        Object* object = nullptr;
        heap_pointer *next = nullptr, *prev = nullptr;
    };

    struct CheneyHeap::Object{
        std::size_t size = 0, header = 0;
        void *field = nullptr;
        heap_pointer *pointers = nullptr;
        virtual Object* _move(char*&) {throw "This should not be called"; };
    };

    class CheneyHeap::SingleObjectBase: public Object {};

    class CheneyHeap::CollectableBase: public Object {
        virtual void move_children(char*&) {throw "This should not be called.\n";}
        virtual void show_children() {throw "This should not be called.\n";}
    };

    template <typename T>
    class CheneyHeap::remote_ptr: public heap_pointer {

    };

    template <typename T>
    class CheneyHeap::SingleObject: public SingleObjectBase{
        Object* _move(char*& to) override {
            return move_object(this, to);
        }
    };

    template <typename T, typename ...Tn>
    class CheneyHeap::Collectable: public CollectableBase {
        std::array<void *, utils::Count<T, Tn...>::count> fields;
        Object* _move(char*& to) override {
            return move_collectable(this, to);
        }
    public:
        void move_children(char*& to) override {
            _move_children<0, T, Tn...>(to);
        }

        template <std::size_t N, typename U, typename ...Un>
        void _move_children(char*& to) {
            if(U::is_ptr) {
                auto c_ptr = reinterpret_cast<heap_pointer *>(fields[N]);
                if(c_ptr->object) {
                    c_ptr->object->_move(to);
                }
            }
            //if(N == 0) return;
            _move_children<N + 1, Un...>();
        }

        template  <std::size_t>
        void _move_children(){; }

    public:
        Collectable() = default;

        template <size_t N>
        auto& get() {
            return *reinterpret_cast<typename utils::TypeHelper<N, T, Tn...>::type::type*>(fields[N]);
        }

        template <typename U, typename ...Args>
        static void construct_self_at(remote_ptr<U> &address, Args&&... args);
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
        using type = remote_ptr<T>;
        constexpr static bool is_ptr = true;
        using is_ptr_type = std::true_type;
    };

    template <typename T>
    using Ptr = CheneyHeap::Ptr<T>;

    template <typename T>
    using Local = CheneyHeap::Local<T>;

    template <typename T>
    CheneyHeap::SingleObject<T>* CheneyHeap::move_object(CheneyHeap::SingleObject<T> *address, char*& to) {
        return nullptr;
    }

    template<typename T, typename... Tn>
    CheneyHeap::Collectable<T, Tn...> *
    CheneyHeap::move_collectable(CheneyHeap::Collectable<T, Tn...> *address, char *&to) {
        return nullptr;
    }
};

#endif //DATA_STRUCTURE_FOR_LOVE_CHENEY_HEAP_HPP
