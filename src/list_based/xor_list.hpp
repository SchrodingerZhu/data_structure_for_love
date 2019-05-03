#ifndef DATA_STRUCTURE_FOR_LOVE_XOR_LIST
#define DATA_STRUCTURE_FOR_LOVE_XOR_LIST
#ifdef DEBUG
#include <iostream>
#include <cstdio>
#endif //DEBUG
#include <memory>
#include <ostream>
#include <object_pool.hpp>

namespace data_structure {

    template <class T>
    struct XorNode { // A XorNode<T>.
        T value; // The Key.
        XorNode *link = nullptr; // The pointer prev ^ the pointer next
        explicit XorNode(const T& v): value(v){}
        template <typename ...Args>
        explicit XorNode(Args&&...args) noexcept : value(std::forward<Args>(args)...)  {}
    };

    template <typename XorList>
    class xor_list_iterator {
    public:
        using value_type = typename XorList::value_type;
        using difference_type = typename XorList::difference_type;
        using iterator_category = std::random_access_iterator_tag;
        using pointer = typename XorList::value_type *;
        using reference = typename XorList::value_type &;
    private:
        XorNode<value_type> *now, *prev;
        difference_type order {};
    public:
        xor_list_iterator(XorNode<value_type>* prev, XorNode<value_type>* now, difference_type order=0) noexcept : now(now), prev(prev), order(order)  {}
        xor_list_iterator &operator++() noexcept {
            ++order;
            auto temp = prev;
            prev = now;
            now = XorList::combine(temp, now->link);
            return *this;
        }
        const xor_list_iterator operator++(int) noexcept {
            ++order;
            auto temp = prev;
            prev = now;
            now = XorList::combine(temp, now->link);
            return *this;
        }
        const xor_list_iterator operator--(int) noexcept {
            order--;
            auto temp = now;
            now = prev;
            prev = XorList::combine(temp, now->link);
            return *this;
        }
        xor_list_iterator& operator--() noexcept {
            order--;
            auto temp = now;
            now = prev;
            prev = XorList::combine(temp, now->link);
            return *this;
        }
        bool operator==(const xor_list_iterator &that) noexcept {
            return this->now == that.now;
        }

        bool operator!=(const xor_list_iterator &that) noexcept {
            return this->now != that.now;
        }
        value_type &operator*() {
            return now->value;
        }
        std::ostream &operator<<(std::ostream& out) const noexcept {
            return out << now;
        }
        difference_type operator-(const xor_list_iterator& that) const noexcept {
            return order - that.order;
        }
        template <class NUM, typename = std::enable_if_t<std::is_integral_v<NUM>>>
        xor_list_iterator operator+(NUM number) const noexcept {
            auto iter {*this};
            while(number--){
                ++iter.order;
                auto temp = iter.prev;
                iter.prev = iter.now;
                iter.now = XorList::combine(temp, iter.now->link);
            }
            return iter;
        }

        template <class NUM, typename = std::enable_if_t<std::is_integral_v<NUM>>>
        xor_list_iterator operator+=(NUM number) noexcept {
            while(number--){
                ++order;
                auto temp = prev;
                prev = now;
                now = XorList::combine(temp, now->link);
            }
            return *this;
        }
        template <class NUM, typename = std::enable_if_t<std::is_integral_v<NUM>>>
        xor_list_iterator operator-(NUM number) const noexcept  {
            auto iter {*this};
            while(number--){
                iter.order--;
                auto temp = iter.now;
                iter.now = iter.prev;
                iter. prev = XorList::combine(temp, iter.now->link);
            }
            return iter;
        }
        bool operator<(const xor_list_iterator& that) const noexcept {
            return order < that.order;
        }
        friend XorList;
    };

    /// @todo: will be finished later
    template <typename XorList>
    class xor_reverse_iterator {
    public:
        using value_type = typename XorList::value_type;
        using difference_type = typename XorList::difference_type;
        using iterator_category = std::random_access_iterator_tag;
        using pointer = typename XorList::value_type *;
        using reference = typename XorList::value_type &;
    private:
        XorNode<value_type> *now, *next;
        difference_type order{};
    public:
        xor_reverse_iterator(XorNode<value_type> *now, XorNode<value_type> *next, difference_type order = 0) : now(now),
                                                                                                               next(next),
                                                                                                               order(order) {}
        xor_reverse_iterator &operator++() {
            auto temp = next;
            next = now;
            now = XorList::combine(temp, now->link);
            return *this;
        }
        const xor_reverse_iterator operator++(int) {
            auto temp = next;
            next = now;
            now = XorList::combine(temp, now->link);
            return *this;
        }
        const xor_reverse_iterator operator--(int) {
            auto temp = now;
            now = next;
            next = XorList::combine(temp, now->link);
            return *this;
        }
        xor_reverse_iterator& operator--() {
            auto temp = now;
            now = next;
            next = XorList::combine(temp, now->link);
            return *this;
        }
        bool operator==(const xor_reverse_iterator &that) {
            return this->now == that.now;
        }

        bool operator!=(const xor_reverse_iterator &that) {
            return this->now != that.now;
        }
        value_type &operator*() {
            return now->value;
        }
        std::ostream &operator<<(std::ostream& out) const {
            return out << now;
        }

        difference_type operator-(const xor_reverse_iterator &that) const noexcept {
            return order - that.order;
        }

        template<class NUM, typename = std::enable_if_t<std::is_integral_v<NUM>>>
        xor_reverse_iterator operator+(NUM number) const noexcept {
            auto iter{*this};
            while (number--) {
                ++iter.order;
                auto temp = iter.next;
                iter.next = iter.now;
                iter.now = XorList::combine(temp, iter.now->link);
            }
            return iter;
        }

        template<class NUM, typename = std::enable_if_t<std::is_integral_v<NUM>>>
        xor_reverse_iterator operator+=(NUM number) noexcept {
            while (number--) {
                ++order;
                auto temp = next;
                next = now;
                now = XorList::combine(temp, now->link);
            }
            return *this;
        }

        template<class NUM, typename = std::enable_if_t<std::is_integral_v<NUM>>>
        xor_reverse_iterator operator-(NUM number) const noexcept {
            auto iter{*this};
            while (number--) {
                iter.order--;
                auto temp = iter.now;
                iter.now = iter.next;
                iter.next = XorList::combine(temp, iter.now->link);
            }
            return iter;
        }

        bool operator<(const xor_reverse_iterator &that) const noexcept {
            return order < that.order;
        }
    };



    template <class T, class Alloc = std::allocator<XorNode<T>>>
    class XorList { // A ;linkedlist
        Alloc memory_pool;
        XorNode<T> *head = nullptr; // Head Pointer
        XorNode<T> *tail = nullptr; // Tail Pointer
        static XorNode<T> *combine(XorNode<T> *a, XorNode<T> *b) {  // XorNode<T>* ^ XorNode<T>*
            return static_cast<XorNode<T> *>(static_cast<ptrdiff_t>(a) ^ static_cast<ptrdiff_t>(b));
        }
        std::size_t _size {};
    public:
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = T &;
        using pointer = T *;
        using iterator = xor_list_iterator<XorList>;
        using reverse_iterator = xor_reverse_iterator<XorList>;
        using const_iterator = const iterator;

        iterator begin() {
            return {nullptr, head};
        }

        iterator end() {
            return {tail, nullptr, static_cast<difference_type>(size())};
        }

        const iterator begin() const {
            return {nullptr, head};
        }

        const iterator end() const {
            return {tail, nullptr, static_cast<difference_type>(size())};
        }

        const iterator cbegin() const {
            return begin();
        }

        const iterator cend() const {
            return end();
        }

        reverse_iterator rbegin() {
            return {tail, nullptr};
        }

        reverse_iterator rend() {
            return {nullptr, head, static_cast<difference_type>(size())};
        }

        const reverse_iterator rbegin() const {
            return {tail, nullptr};
        }

        const reverse_iterator rend() const{
            return {nullptr, head, static_cast<difference_type>(size())};
        }

        const reverse_iterator crbegin() const {
            return rbegin();
        }

        const reverse_iterator crend() const{
            return rend();
        }


        size_type size() {
            return _size;
        }

        void push_back(const value_type & newValue) {
            auto *newNode = memory_pool.allocate(1);
            memory_pool.construct(newNode, newValue);
            if (head == nullptr) {
                head = newNode;
                tail = newNode;
            } else {
                newNode->link = combine(tail, nullptr);
                tail->link = combine(combine(tail->link, nullptr), newNode);
                tail = newNode;
            }
            _size += 1;
        }

        template <typename ...Args>
        void emplace_back(Args&&... args) {
            auto *newNode = memory_pool.allocate(1);
            memory_pool.construct(newNode, std::forward<Args>(args)...);
            if (head == nullptr) {
                head = newNode;
                tail = newNode;
            } else {
                newNode->link = combine(tail, nullptr);
                tail->link = combine(combine(tail->link, nullptr), newNode);
                tail = newNode;
            }
            _size += 1;
        }
#ifdef DEBUG
        void view() { // Print out the list.
            if (head == nullptr)
                return;
            viewList(head, nullptr);
        }

        void viewList(XorNode<value_type> *now, XorNode<value_type> *last) {
            std::cout << now->value << std::endl;
            if (now == tail) {
                //cout << endl;
                return;
            }
            viewList(combine(now->link, last), now);
        }
#endif //DEBUG
        void pop_back() { // Delete a XorNode<T>.
            if (head == nullptr)
                return;
            else {
                auto prev = combine(tail->link, nullptr);
                memory_pool.destroy(tail);
                memory_pool.deallocate(tail, 1);
                if (prev) prev->link = combine(combine(prev->link, tail), nullptr);
                tail = prev;
                _size--;
            }
        }

//        void erase(size_type n) {
//            if(n >= size()) return;
//            auto next = begin() + 1;
//            auto now  = begin();
//            while(n--) {
//                now++;
//                next++;
//            }
//            if(now.now == head) {
//                head = next.now;
//            } else {
//                now.prev->link = combine(next.now, combine(now.now, now.prev->link));
//            }
//            next.now->link = combine(now.prev, combine(now.now, next.now->link));
//
//            if(now.now == tail) {
//                tail == now.prev;
//            }
//        }

        bool contains(const value_type &target) { // Find out if a XorNode<T> exists.
            auto iter = begin(), e = end();
            while (iter < e) {
                if (*iter == target) return true;
                ++iter;
            }
            return false;
        }

        friend iterator;
        friend reverse_iterator;
    };

}

namespace std {

}
#endif //DATA_STRUCTURE_FOR_LOVE_XOR_LIST