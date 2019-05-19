# Project Report

## Project Information

### Title General 

Data Structures Library for Modern C++

### Contributors

|   Name     | ID | Section |
|--------|----------|-------|
|  Yifan ZHU | 118010469 | L01|
|Jin LIU| 116010144|L02|
|Yuncong CUI| 118010045 | L02|
|Jingui DING| 117010052 | L02|

### Coding Amount
Up to the report, $116$ commits have been pushed in $11$ branches.
- C++ related line count (including cmake): 11111 lines in 72 files

- All line count: 1946721 in 157 files 

### Code Coverage
![codecov](https://codecov.io/gh/SchrodingerZhu/data_structure_for_love/branch/master/graph/badge.svg)

![Coverage Pictue](https://codecov.io/gh/SchrodingerZhu/data_structure_for_love/branch/master/graphs/sunburst.svg)

## Project Manual

### How to use

This is a data structure library for learning, research and industry purpose. You can directly add our code to your project file and
use it.

The check the whole project. You can clone it, `cd` to the path and use the following command

```bash
mkdir Test
cd Test
cmake ..
make
make test
```

The implementation part is sealed in `namespace` `data_structure` . If you want to customize some data structure, 
some useful classes and functions are in `data_structure::utils`.

### Interfaces

#### Array Like
##### optimized_vector
The public interfaces
```c++
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = T &;
        using pointer = T *;
        using iterator = T *;
        using const_iterator = T const *;
        // default constructor
        optimized_vector() noexcept;

        explicit optimized_vector(size_type count, value_type const &value = value_type());

        optimized_vector(optimized_vector const &that);

        optimized_vector(optimized_vector &&that) noexcept;

        optimized_vector(std::initializer_list<T> init_list);

        ~optimized_vector();

        iterator begin() noexcept;

        iterator end() noexcept;

        inline const_iterator begin() const noexcept;

        inline const_iterator end() const noexcept { return cend(); }

        const_iterator cbegin() const;

        const_iterator cend() const;

        value_type &operator[](size_type n) { return *(begin() + n); }

        value_type const &operator[](size_type n) const { return *(cbegin() + n); }

        size_type size() const noexcept { return cend() - cbegin(); }

        size_type capacity() const noexcept;

        void push_back(value_type const &value);

        void push_back(value_type &&value);

        template<typename ...Args>
        void emplace_back(Args &&...args);

        void pop_back();

        void erase(size_type position);

        void clear();

        void resize(const size_t &target, value_type const &value = value_type());

        void empty_fill_local();

        bool empty();

        T &back();
```
Example:
```c++
data_structure::optimized_vector<int, 500> test;
test.resize(333, 991208);
```
Optimized array is almost the same as `std::vector`, but for small amount of data, it will directly store them in local memory.
Therefore, it can be faster. For known sized data, you can directly provide the size and ask the array to store them all in local.
For large amount of data, it is recommended to determine the local size wisely - because it can waste a lot of space if the data size grow
over the local size if the size is set considerably large.

##### Rootish Stack
Interfaces:
```c++
        size_t size() const noexcept;

        void push_back(const T &i);

        void clear() noexcept;

        T &operator[](size_t index) noexcept;

        class iterator;

        friend class iterator;

        iterator begin() noexcept;

        iterator end() noexcept;

        void erase(size_t index);

        ~RootishStack();
```

This is a stack-like array, because it will be must faster if you operate your data near the end. The biggest difference between
Rootish Stack and normal vector is that the free space (wasted space) is upper bounded by $O(\sqrt n)$. But the drawback is random accessing now involves floating
number calculation which is costly. We provide two calculation plan, the first way is to use the `sqrt` function in `<cmath>` and the second way is to use the famous sqrt inverse algorithm.
The latter one can be useful if your CPU does not support fast sqrt calculation like SSE set. To switch between the two plans, you can do as following:
```c++
RootishStack<int, vector<int*>, NormalSqrt> a;
RootishStack<int, vector<int*>, InverseSqrt> a;
```
As you can see, the second template parameters can also be changed if you want to choose your own container to store the pointers.

#### Binary Tree
Binary Trees are designed to have high ability to be customized. Your can provide your own `Node` class to collect more information on the tree.
Your own `Node` should inherit the `struct Node` in `binary_tree_base.hpp` and provide a `update()` function which can update your info according to its two children.
We use "bottom-up" method to maintain the tree so please make sure your update has "bottom-up" property. The get your own data, you can use our `locate` method to get a tree walker of that node and then
retrieve the data in the way you like.

The tree walker interfaces are
```c++
        explicit walker(Node *node) : node(node) {}

        Node *unsafe_cast() { return node; }

        T &get() { return node->x; }

        const T &get() const { return node->x; }

        T &operator*() { return get(); }

        const T &operator*() const { return get(); }

        T *operator->() { return &get(); }

        const T *operator->() const { return &get(); }

        void go_up() { node = static_cast<Node *>(node->parent); }

        void go_left() { node = static_cast<Node *>(node->children[LEFT]); }

        void go_right() { node = static_cast<Node *>(node->children[RIGHT]); }

        void go_min() { node = min_node(node); }

        void go_max() { node = max_node(node); }

        void go_succ() { node = succ_node(node); }

        void go_pred() { node = succ_node(node); }

        bool valid() { return node; }
```
Be gentle to use that `unsafe_cast` method and do care about the in-order property of your lovely tree!
The template parameters for binary trees are 
```c++
   template<class T, class Node = BSTNode<T>,
               class Compare = DefaultCompare<T>, class Factory = utils::PoolFactory<Node>>
```

While the first two parameters are trivial to understand, the `Compare` parameter determines a functor that will be used to find the partial order
relation between two element. It must return an element in the `Relation` enum, including "Less, Eq, Greater". And the last parameter is to determine which Node allocator should be used.
We provide `utils::TrivialFactory` and `utils::PoolFactory`. The first one uses `new/delete` to construct node and second one is a wrapper of `ObjectPool`. You can create your own one if you like.
Currently no tree merging operation is supported in any variant of BST, but we may add some later. Therefore, node factory will be useful in describing how we should do with the memory space if two trees are merged. 

Binary Trees support many useful methods：
```c++
        class walker;

        class iterator;

        virtual bool insert(const T &x);

        virtual bool erase(const T &x);

        virtual walker find(const T &x);

        virtual size_t size();

        virtual void clear();

        virtual walker min();

        virtual walker max();

        virtual walker top();

        virtual walker pred(const T &x);

        virtual walker locate(const T& x);

        virtual walker succ(const T &x);

        virtual bool contains(const T &x);

        iterator begin();

        iterator end();
```
##### AVL
`AVLTree` is an interesting variant of balanced BST. Its amortized height is even smaller than red black tree and thus provide good performance in many cases [0].

##### Red Black Tree
`RbTree` is the most widely used balanced BST. The version we implemented is the so called left-leaning red black tree, which guarantees that only the nodes one the searching path will be rotated during re-balance.
If the `update` is costly, `RbTree` may be a better choice [0].

##### Scapegoat Tree
`Scapegoart` is a representative of a weight balanced BST. It uses brute force rebuilding method but as the re-balance frequency is low, it provide a good performance. Weight balance trees are very useful in some cases, like acting as a helper in high dimensional searching [1].

##### Splay
`Splay` uses a wise way to rotate the nodes via searching. Is is statically optimal and has Working Set Property together with  Dynamic Finger Property [1]. Thus for a relatively ordered searching sequence, splay may be the best choice.   

##### Treap
`Treap` maintains a heap property of a random generated priority value for each node. Thus is can simulate random insertion.

#### Heap

The base class of heap is

```c++
    template<typename T, typename Compare = std::less<T>>
    class Heap {
    public:
        virtual const T &top() = 0;
        virtual void push(const T& t) = 0;

        virtual size_t size() const = 0;
        virtual void pop() = 0;

        virtual bool empty() = 0;
    };
```

Every variant must implement these method. Also, an `emplace` method is supported in all kinds of heaps. Since it uses the variant length template argument list, the base class cannot contain this method.

##### Binary Heap

`BinaryHeap` is a partial ordered tree. The template parameters are:

```c++
    template<typename T,
            typename Compare = std::less<T>,
            typename Alloc = std::allocator<BinaryHeapNode<T>>>
```

`Compare` is a functor to judge the partial order relationship of two elements and `Alloc` is the allocator of the nodes.

##### Other Variants

`BinomialHeap`, `ParingHeap` and `FibHeap` support merge and decreasing. You can also choose your own `Compare`. However, as a merge operation is there,  we simply use `new/delete` is manage the memory. We may migrate them into node factories later.

##### Complexity

| heap |insert | top  | pop  | merge | decrease |
| -------- | ---- | ---- | ----- | -------- |-----|
| BinaryHeap | $O(\lg N)$ | $O(1)$ |  $O(\lg N)$     |  X        |X|
| BinomialHeap | $O(1)$ | $O(\lg N)$|$O(\lg N)$ |$O(\lg N)$ |$O(\lg N)$|
| PairingHeap |$O(1)$ | $O(1)$ | $O(\lg N)$ | $O(1)$ |$O(\lg N)$|
| FibHeap |$O(1)$ | $O(1)$ | $O(\lg N)$ | $O(1)$|$O(1)$|

#### Integer

`IntegerSet`  section want to provide several ways to indexing and searching $w-bit$ word integers.

The base class is 

```c++
template<typename Int, typename = std::enable_if_t<std::is_integral_v<Int>>>
    class IntegerSetBase {
    public:
        virtual bool contains(Int t) const = 0;

        virtual std::optional<Int> pred(Int t) const = 0;

        virtual std::optional<Int> succ(Int t) const = 0;

        virtual std::optional<Int> max() const = 0;

        virtual std::optional<Int> min() const = 0;

        virtual void insert(Int t) = 0;

        virtual void erase(Int t) = 0;
    };
```

All the variants support the same public method while the second template parameter in each variant is the bit length of the integral word.

##### Binary Trie

`BinaryTrie` is very simple. It branches on each bit and therefore form a way to rapidly indexing the integer in log time. By chaining the leaf node together, it is also easy for us to iterate to whole set.

##### XFastTrie

`XFastTrie` speed up the searching time to doubly log by using hashing and binary search. However, the hash map in STL is very slow. Thus the performance of `XFastTire` is not good. Therefore, we provide a third template parameter to let you choose the hash container yourself.

##### YFastTire

`YFastTrie` again speed up the insertion and deletion also to doubly log by using a special treap on its nodes. However, the hash map in STL is very slow. Thus the performance of `YFastTire` is not good. Therefore, we provide a third template parameter to let you choose the hash container yourself.

##### Van Emde Boas

`VebTree` is an implement of `RS-Van Emde Boas Tree`.  All of its operations are in doubly log time.

#### List Based

Lists' template parameters are

```c++
template<class T, class Alloc = std::allocator<Node<T>>>
```

##### Singly Linked List

The interfaces are 

```c++
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

        void pop_back();

        void erase(size_type n);

        bool contains(const value_type &target);

        friend iterator;
```

It is quite similar to `std::forward_list`, but we support `push_back`.

##### Xor List

One draw back of doubly linked list (`std::list`) is that one node needs two pointers to connect. It may waste a lot of space. `XorList` decrease the number of pointer to one. The interfaces are

```c++
		using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = T &;
        using pointer = T *;
        using iterator = xor_list_iterator<XorList>;
        using reverse_iterator = xor_reverse_iterator<XorList>;
        using const_iterator = const iterator;

        iterator begin();

        iterator end();

        const iterator begin() const;

        const iterator end() const;

        const iterator cbegin() const;

        const iterator cend() const;

        reverse_iterator rbegin();

        reverse_iterator rend();

        const reverse_iterator rbegin() const;

        const reverse_iterator rend() const;

        const reverse_iterator crbegin() const;

        const reverse_iterator crend() const;


        size_type size();

        void push_back(const value_type &newValue);

        template<typename ...Args>
        void emplace_back(Args &&... args);

        void pop_back();

        void erase(size_type n);

        bool contains(const value_type &target);

        friend iterator;
        friend reverse_iterator;
```

#### Memory

**NOTICE: CheneyHeap is not under maintenance in current stage.**

##### Object Pool

`ObjectPool` is designed to reduce the new operation and support memory recycle.

The interfaces are



```c++
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef const T &const_reference;
        typedef T value_type;

        template<typename U>
        struct rebind {
            typedef ObjectPool<U> other;
        };

#if __cplusplus >= 201103L
        typedef std::true_type propagate_on_container_move_assignment;

        typedef std::true_type is_always_equal;
#endif

        ObjectPool() = default;

        ObjectPool(ObjectPool const &that) = delete;

        ObjectPool &operator=(ObjectPool const &that) = delete;

        void absorb(ObjectPool &that);

        ObjectPool(ObjectPool &&that) noexcept;

        ObjectPool &operator=(ObjectPool &&that) noexcept;

        [[nodiscard]] T *get_raw();

        [[nodiscard]] T *allocate(size_type t);

        template<typename ...Args>
        [[nodiscard]] T *construct_raw(Args &&...args);;

        template<typename ...Args>
        void construct(T *p, Args &&...args);

        inline void recycle(T *address);

        inline void destroy(T *address);

        inline void deallocate(T *address, size_type n);

        std::shared_ptr<T> get_shared();

        template<typename ...Args>
        std::shared_ptr<T> construct_shared(Args &&...args);;

        ~ObjectPool();
```

It can be directly used as an allocator as it also implements STL `allocator_trait`

 The template parameters are

```c++
template<typename T, std::size_t ChunkSize = 1000,
            typename PtrContainer = std::vector<T *>>
```

You can tweak the `ChunkSize` and use your own `PtrContainer` if you want. The `absorb` operation can be costly. Be careful to use. 

## Project Benchmark

### How to benchmark

At `misc/benchmark` we provide some self-written benchmark class. You can inherit the `BenchMark` base class, and it will register the benchmark automatically.

### Benchmark #1 Memory Allocation

![alloc](misc/benchmark/picture/Alloc.png) 

The progress including allocating the given size of memory and then deallocate it. By pre-allocate enough space, the `ObjectPool` is much better.

### Benchmark #2 Binary Tree Random Checking

![](misc/benchmark/picture/bst-random-checking.png)

In random checking part, `Splay` get harm due to its frequent rotation. `AVLTree` and `RBTree` have the best performance (even a little bit faster than `std::set`).

### Benchmark #3 Binary Tree Random Iteration

![](misc/benchmark/picture/bst-random-iter.png)

There is no huge different in iteration part among BSTs. `RbTree` and `AVLTree` still are the best ones, while `RbTree` is more stable.

### Benchmark #4 Binary Tree Random Insetion

![](misc/benchmark/picture/bst-random-insert.png)

 ### Benchmark #5 Binary Tree Random Deletion

![](misc/benchmark/picture/bst-random-del.png)

### Benchmark #6 Binary Tree Ordered Insertion

![](misc/benchmark/picture/bst-ordered-insert.png)

In ordered insertion, however, `Splay` and `Treap` are very efficient. `Scapegoat` is much lower in this part.

### Benchmark #7 Binary Tree Ordered Checking

![](misc/benchmark/picture/bst-ordered-check.png)

In this part, we insert `n` value randomly and then checking `1 to 1000000` in order. `Splay` seems not to depend on the number of inserted nodes.

### Benchmark #8 Array

![](misc/benchmark/picture/vectorcomparison.png)

The test includes `push_back` and random access.

### Benchmark #9 Heap Insertion

![](misc/benchmark/picture/heap-insertion.png)

### Benchmark #10 Heap Popping

![](misc/benchmark/picture/heap-popping.png)

### Benchmark #11 Heap Merging

![](misc/benchmark/picture/heap-merge.png)

### Benchmark #12 Heap Decreasing Key

Decreasing key in paring heap is kind of messy and therefore is moved into another figure.

![](misc/benchmark/picture/dec0.png)

![](misc/benchmark/picture/dec1.png)

### Benchmark #12 Intset Insertion

![](misc/benchmark/picture/int-ins.png)



### Benchmark #13 Intset Deletion

![](misc/benchmark/picture/int-del.png)

### Benchmark #14 Intset Finding Extrema

![](misc/benchmark/picture/int-ex.png)

### Benchmark #15 Intset Iteration

![](misc/benchmark/picture/int-iter.png)

## Our Strengths

- every data structure support high freedom of customization.
- the whole project is organized organically
- high order c++ features like semantic move, SFINAE, std::optional are involved to provide better experience and performance.

## Reference

\[0] : Open Data Structures (http://opendatastructures.org/)

[1] :  Advanced Data Structures by Prof. [Erik Demaine](http://erikdemaine.org/) (http://courses.csail.mit.edu/6.851/)

[2] :  Elementary Algorithms by Larry [LIU Xinyu](https://github.com/liuxinyu95/) (https://github.com/liuxinyu95/AlgoXY)