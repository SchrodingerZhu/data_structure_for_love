//
// Created by schrodinger on 2/22/19.
//
// TODO: Change children to hash multiset to handle one-father-many-pointers situation
#ifndef DATA_STRUCTURE_FOR_LOVE_CHENEY_HEAP_HPP
#define DATA_STRUCTURE_FOR_LOVE_CHENEY_HEAP_HPP

#include <cstddef>
#include <vector>
#include <unordered_set>
#include <object_management.hpp>
#include <memory>
#ifdef DEBUG
#include <iostream>
#endif // DEBUG

namespace data_structure {
    template<class T>
    class cheney_ptr;

    template<class T>
    class CheneyHeap;

    template<class T>
    class cheney_ptr {
    private:
        cheney_ptr *next = nullptr, *prev = nullptr, *father = nullptr;
        using Object = typename CheneyHeap<T>::Object;
        Object *address = nullptr;

        explicit cheney_ptr(Object *raw_ptr, cheney_ptr* father) {
            address = raw_ptr;
            this->father = father;
            if (raw_ptr->pointer == nullptr) {
                raw_ptr->pointer = this;
            } else {
                raw_ptr->pointer->add_next(this);
            }
        }

    public:
        ~cheney_ptr() {
            if (next) next->prev = prev;
            if (prev) prev->next = next;
            if (address && address->pointer == this) {
                address->pointer = this->next;
            }
            if (father && father->address) {
                father->address->children.erase(this->address);
            }
            if (address && address->pointer == nullptr) {
               utils::destroy_at(address);
            }
        }

        cheney_ptr(const cheney_ptr  &that, cheney_ptr const &father = cheney_ptr(nullptr)) {
            this->address = that.address;
            if(const_cast<cheney_ptr &>(father) == nullptr) {
                this->father = that.father;
            } else {
                this->father = const_cast<cheney_ptr *>(&father);
                if(this->father->address != that.father->address)
                    this->father->address->add_children(this->address);
            }
            const_cast<cheney_ptr &>(that).add_next(this);
        }

        explicit cheney_ptr(const std::nullptr_t& t) {
            address = nullptr;
        }



        cheney_ptr(cheney_ptr &&that) noexcept {
            next = that.next;
            prev = that.prev;
            father = that.father;
            address = that.address;
            if (that.prev) {
                that.prev->next = this;
            }
            if (that.next) {
                that.next->prev = this;
            }
            that.father = that.next = that.prev = nullptr;
            that.address = nullptr;
        }

        T &operator*() {
            return *reinterpret_cast<T *>(&address->storage);
        }

        bool operator==(std::nullptr_t) {
            return address == nullptr;
        }

        bool operator!=(std::nullptr_t) {
            return address != nullptr;
        }

        void add_next(cheney_ptr *that) {
            that->next = next;
            that->prev = this;
            if (next) {
                next->prev = that;
            }
            next = that;
        }

        friend CheneyHeap<T>;
    };

    template<typename T>
    class CheneyHeap {
        std::size_t heap_size;
        struct Object {
#ifdef DEBUG
            bool visited = false;
#endif
            cheney_ptr<T> *pointer;
            std::unordered_set<struct Object *> children;
            std::aligned_storage_t<sizeof(T), alignof(T)> storage;
            CheneyHeap* heap;
            explicit Object(CheneyHeap* heap): heap(heap) {}
            Object(Object&& obj) noexcept {
                pointer = obj.pointer;
                children = std::move(obj.children);
                storage = std::move(obj.storage);
                heap = obj.heap;
            }
            ~Object() {
                auto p = pointer;
                while (p) {
                    if (pointer->father->address->children.count(p->address)) {
                        pointer->father->address->children.erase(p->address);
                    }
                    p = p->next;
                }
                T *a = reinterpret_cast<T *>(&storage);
                utils::destroy_at(a);
            }

            void add_children(Object *that) {
                children.insert(that);
            }
        } *heap = nullptr, *free = nullptr, *from = nullptr, *to = nullptr;
        Object* copy(Object* obj) {
            utils::emplace_construct(free, std::move(*obj));
            auto pointer = obj->pointer;
            while (pointer) {
                pointer->address = free;
                pointer = pointer->next;
            }
            return free++;
        }


        void grow_heap() {

        };

        Object *get_raw_pointer(cheney_ptr<T>* father) {
            if (free - from > heap_size) garbage_collect();
            if (free - from > heap_size) grow_heap();
            utils::emplace_construct(free, this);
            if (*father == nullptr) {
                roots.insert(free);
            } else {
                father->address->add_children(free);
            }
            return free++;
        }

        std::unordered_set<Object *> roots;
    public:

        explicit CheneyHeap(std::size_t init_size = 1000) {
            heap_size = init_size;
            heap = reinterpret_cast<Object *>(::operator new(heap_size * sizeof(Object) << 1));
            free = from = heap;
            to = from + heap_size;
        }

        ~CheneyHeap() {
            ::operator delete(reinterpret_cast<void*>(heap));
        }

        cheney_ptr<T> get(cheney_ptr<T> const &father = cheney_ptr<T>(nullptr)) {
            auto ptr = get_raw_pointer(const_cast<cheney_ptr<T> *>(&father));
            return cheney_ptr<T>(ptr, const_cast<cheney_ptr<T> *>(&father));
        }
        void garbage_collect() {
            Object* scan = free = to;
            int n = 0;
            auto temp = roots;
            for(auto i: temp) {
                roots.erase(i);
                auto address = copy(i);
                roots.insert(address);
            }

            while(scan != free) {
                auto fake = scan->children;
                for(auto i: fake) {
                    scan->children.erase(i);
                    auto address = copy(i);
                    scan->children.insert(address);
                }
                scan++;
            }
            std::swap(from, to);
        };

#ifdef DEBUG
        std::unordered_set<Object*> visits;

        void debug() {
            for (auto i: roots) {
                debug(i);
            }
        }

        void debug(cheney_ptr<T> &ptr) {
            debug(ptr.address);
        }

        void debug(Object *ptr) {
            ptr->visited = true;
            visits.insert(ptr);
            std::cout << "\n----------------------------\n";
            std::cout << "address: " << ptr << "\n";
            std::cout << "content: " << *reinterpret_cast<T *>(&ptr->storage) << "\n";
            auto pointer = ptr->pointer;
            size_t n = 0;
            std::unordered_set<cheney_ptr<T> *> fathers;
            while (pointer) {
                n++;
                if (pointer->father)
                    fathers.insert(pointer->father);
                pointer = pointer->next;
            }
            std::cout << "pointer_num: " << n << "\nfathers: ";
            for (auto i: fathers) {
                std::cout << i->address << " ";
            }
            std::cout << std::endl;
            auto children = ptr->children;
            std::cout << "children_num: " << children.size() << std::endl;
            std::cout << "children: ";
            for (auto i: children) {
                std::cout << i << " ";
            }
            std::cout << "\n----------------------------\n";
            for(auto i: children)
                if(!i->visited) {
                    debug(i);
                }
        }

        void clear_visits() {
            for(auto i: visits) {
                i->visited = false;
            }
            visits.clear();
        }
#endif //
        friend cheney_ptr<T>;
    };


};

#endif //DATA_STRUCTURE_FOR_LOVE_CHENEY_HEAP_HPP
