//
// Created by schrodinger on 19-5-19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_SINGLE_LINKED_LIST_HPP
#define DATA_STRUCTURE_FOR_LOVE_SINGLE_LINKED_LIST_HPP

#include <iostream>
#include <cstdio>
#include <memory>
#include <object_pool.hpp>

namespace data_structure {

    template<class T>
    struct Node { // A Node<T>.
        T value; // The Key.
        Node *next = nullptr;

        explicit Node(const T &v) : value(v) {}

        template<typename ...Args>
        explicit Node(Args &&...args) noexcept : value(std::forward<Args>(args)...) {}
    };


    template<typename SingleList>
    class list_iterator {
    public:
        using value_type = typename SingleList::value_type;
        using difference_type = typename SingleList::difference_type;
        using iterator_category = std::random_access_iterator_tag;
        using pointer = typename SingleList::value_type *;
        using reference = typename SingleList::value_type &;
    private:
        Node<value_type> *now, *prev;
        difference_type order{};
    public:
        list_iterator(Node<value_type> *prev, Node<value_type> *now, difference_type order = 0) noexcept :
                now(now), prev(prev), order(order) {}

        list_iterator &operator++() noexcept {
            ++order;
            prev = now;
            now = now->next;
            return *this;
        }

        const list_iterator operator++(int) noexcept {
            auto temp = *this;
            ++order;
            prev = now;
            now = now->next;
            return temp;
        }

        bool operator==(const list_iterator &that) noexcept {
            return this->now == that.now;
        }

        bool operator!=(const list_iterator &that) noexcept {
            return this->now != that.now;
        }

        value_type &operator*() {
            return now->value;
        }

        std::ostream &operator<<(std::ostream &out) const noexcept {
            return out << now;
        }

        difference_type operator-(const list_iterator &that) const noexcept {
            return order - that.order;
        }

        template<class NUM, typename = std::enable_if_t<std::is_integral_v<NUM>>>
        list_iterator operator+(NUM number) const noexcept {
            auto iter{*this};
            while (number--) {
                ++iter.order;
                auto temp = iter.prev;
                iter.prev = iter.now;
                iter.now = iter.now->next;
            }
            return iter;
        }

        template<class NUM, typename = std::enable_if_t<std::is_integral_v<NUM>>>
        list_iterator operator+=(NUM number) noexcept {
            while (number--) {
                ++order;
                auto temp = prev;
                prev = now;
                now = now->next;
            }
            return *this;
        }

        template<class NUM, typename = std::enable_if_t<std::is_integral_v<NUM>>>
        list_iterator operator-(NUM number) const noexcept {
            auto iter{*this};
            while (number--) {
                iter.order--;
                auto temp = iter.now;
                iter.now = iter.prev;
                iter.prev = iter.now->next;
            }
            return iter;
        }

        bool operator<(const list_iterator &that) const noexcept {
            return order < that.order;
        }

        friend SingleList;
    };

    template<class T, class Alloc = std::allocator<Node<T>>>
    class SingleList {
        Alloc memory_pool;
        Node<T> *head = nullptr; // Head Pointer
        Node<T> *tail = nullptr; // Tail Pointer
        std::size_t _size{};
    public:
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = T &;
        using pointer = T *;
        using iterator = list_iterator<SingleList>;
        using const_iterator = const iterator;

        iterator begin();

        iterator end();

        const iterator begin() const;

        const iterator end() const;

        const iterator cbegin() const;

        const iterator cend() const;

        size_type size();

        void push_back(const value_type &newValue);

        template<typename ...Args>
        void emplace_back(Args &&... args);

/*
        void view() { // Print out the list.
            if (head == nullptr)
                return;
            viewList(head);
        }

        void viewList(Node<value_type> *now) {
            std::cout << now->value << std::endl;
            if (now == tail) {
                //cout << std::endl;
                return;
            }
            viewList(next);
        }
*/

        void pop_back();

        void erase(size_type n);

        bool contains(const value_type &target);

        friend iterator;
    };

    template<class T, class Alloc>
    typename SingleList<T, Alloc>::iterator SingleList<T, Alloc>::begin() {
        return {nullptr, head};
    }

    template<class T, class Alloc>
    typename SingleList<T, Alloc>::iterator SingleList<T, Alloc>::end() {
        return {tail, nullptr, static_cast<difference_type>(size())};
    }

    template<class T, class Alloc>
    const typename SingleList<T, Alloc>::iterator SingleList<T, Alloc>::begin() const {
        return {nullptr, head};
    }

    template<class T, class Alloc>
    const typename SingleList<T, Alloc>::iterator SingleList<T, Alloc>::end() const {
        return {tail, nullptr, static_cast<difference_type>(size())};
    }

    template<class T, class Alloc>
    const typename SingleList<T, Alloc>::iterator SingleList<T, Alloc>::cbegin() const {
        return begin();
    }

    template<class T, class Alloc>
    const typename SingleList<T, Alloc>::iterator SingleList<T, Alloc>::cend() const {
        return end();
    }

    template<class T, class Alloc>
    typename SingleList<T, Alloc>::size_type SingleList<T, Alloc>::size() {
        return _size;
    }

    template<class T, class Alloc>
    void SingleList<T, Alloc>::push_back(const value_type &newValue) {
        auto *newNode = memory_pool.allocate(1);
        memory_pool.construct(newNode, newValue);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            newNode->next = nullptr;
            tail->next = newNode;
            tail = newNode;
        }
        _size += 1;
    }

    template<class T, class Alloc>
    template<typename... Args>
    void SingleList<T, Alloc>::emplace_back(Args &&... args) {
        auto *newNode = memory_pool.allocate(1);
        memory_pool.construct(newNode, std::forward<Args>(args)...);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            newNode->next = nullptr;
            tail->next = newNode;
            tail = newNode;
        }
        _size += 1;
    }

    template<class T, class Alloc>
    void SingleList<T, Alloc>::pop_back() { // Delete a Node<T>.
        if (head == nullptr)
            return;
        else {
            memory_pool.destroy(tail);
            memory_pool.deallocate(tail, 1);
            if (head != tail) {
                Node<value_type> *find = head;
                while (find->next != tail)
                    find = find->next;
                find->next = nullptr;
                tail = find;
            } else {
                head == nullptr;
                tail == nullptr;
            }
            _size--;
        }
    }

    template<class T, class Alloc>
    void SingleList<T, Alloc>::erase(SingleList::size_type n) {
        if (n >= size() || n < 0) return;
        if (n == 0) {
            head = head->next;
            if (n == size() - 1)
                tail == nullptr;
            _size--;
            return;
        }
        auto now = begin();
        while (--n) {
            now++;
        }
        if (now.now->next == tail)
            tail == now.now;
        now.now->next = now.now->next->next;
        _size--;
    }

    template<class T, class Alloc>
    bool SingleList<T, Alloc>::contains(const value_type &target) { // Find out if a Node<T> exists.
        auto iter = begin(), e = end();
        while (iter < e) {
            if (*iter == target) return true;
            ++iter;
        }
        return false;
    }

}
#endif //DATA_STRUCTURE_FOR_LOVE_SINGLE_LINKED_LIST_HPP
