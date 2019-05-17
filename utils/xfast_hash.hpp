//
// Created by schrodinger on 19-4-22.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_XFAST_HASH_HPP
#define DATA_STRUCTURE_FOR_LOVE_XFAST_HASH_HPP

#include <new>
#include <unordered_map>
namespace data_structure::utils {

    template<class T>
    class BitHashBase {
    public:
        virtual T get(size_t index) = 0;

        virtual void put(size_t index, T m) = 0;

        virtual ~BitHashBase() = default;
    };

    template<class T, size_t N, bool = true>
    class BitHashImpl : public BitHashBase<T> {
        T mem[(1u << N)];
    public:
        BitHashImpl() {
            std::memset(mem, 0, sizeof(mem));
        }

        T get(size_t index) {
            return mem[index];
        }

        void put(size_t index, T m) {
            mem[index] = m;
        }
    };

    template<class T, size_t N>
    using BitHash = BitHashImpl<T, N, (N <= 8)>;


    template<class T, size_t N>
    class BitHashImpl<T, N, false> : public BitHashBase<T> {
        BitHash<T, N - 8> *mem[(1u << 8u)];
    public:
        BitHashImpl() {
            std::memset(mem, 0, sizeof(mem));
        }

        T get(size_t index) {
            auto shifted = index >> 8u;
            auto mask = index & ((1u << 8u) - 1);
            if (!mem[mask]) return T{};
            else return mem[mask]->get(shifted);
        }

        void put(size_t index, T m) {
            auto shifted = index >> 8u;
            auto mask = index & ((1u << 8u) - 1);
            if (!mem[mask]) mem[mask] = new BitHash<T, N - 8>;
            mem[mask]->put(shifted, m);
        }

        ~BitHashImpl() override {
            for (auto i: mem) { if (i) delete i; }
        }
    };

    template<class T, size_t current>
    struct _Builder : _Builder<T, current - 1> {
        using type = BitHashBase<T>;

        explicit _Builder(type **addr) : _Builder<T, current - 1>(addr - 1) {
            *addr = static_cast<BitHashBase<T> *>(::operator new(sizeof(BitHash<T, current>)));
            emplace_construct<BitHash<T, current>>(static_cast<BitHash<T, current> *>(*addr));
        }

    };

    template<class T>
    struct _Builder<T, 0> {
        using type = BitHashBase<T>;

        explicit _Builder(type **addr) {}
    };


    template<class Key, class U>
    class STL_WRAPPER : public std::unordered_map<Key, U> {
    public:
        inline U get(Key index) noexcept { return this->operator[](index); }

        inline void put(Key index, U u) noexcept { this->operator[](index) = u; }
    };

    template<class T, size_t current>
    struct STL_Builder : STL_Builder<T, current - 1> {
        using type = STL_WRAPPER<size_t, T>;

        explicit STL_Builder(type **addr) : STL_Builder<T, current - 1>(addr - 1) {
            *addr = static_cast<type *>(::operator new(sizeof(type)));
            emplace_construct<type>(*addr);
        }

    };

    template<class T>
    struct STL_Builder<T, 0> {
        using type = STL_WRAPPER<size_t, T>;

        explicit STL_Builder(type **addr) {}
    };

}
#endif //DATA_STRUCTURE_FOR_LOVE_XFAST_HASH_HPP
