//
// Created by schrodinger on 19-4-22.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_XFAST_HASH_HPP
#define DATA_STRUCTURE_FOR_LOVE_XFAST_HASH_HPP

#include <new>

namespace data_structure::utils {

    template<class T>
    class BitHashBase {
    public:
        virtual T get(size_t index) = 0;

        virtual void put(size_t index, T m) = 0;
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
    using BitHash = BitHashImpl<T, N, (N <= 6)>;


    template<class T, size_t N>
    class BitHashImpl<T, N, false> : public BitHashBase<T> {
        BitHash<T, N - 6> *mem[(1u << 6u)];
    public:
        BitHashImpl() {
            std::memset(mem, 0, sizeof(mem));
        }

        T get(size_t index) {
            auto shifted = index >> 6u;
            auto mask = index & ((1u << 6u) - 1);
            if (!mem[mask]) return T{};
            else return mem[mask]->get(shifted);
        }

        void put(size_t index, T m) {
            auto shifted = index >> 6u;
            auto mask = index & ((1u << 6u) - 1);
            if (!mem[mask]) mem[mask] = new BitHash<T, N - 6>;
            mem[mask]->put(shifted, m);
        }

        ~BitHashImpl() {
            for (auto i: mem) { if (i) delete i; }
        }
    };

    template<class T, size_t current>
    struct _Builder : _Builder<T, current - 1> {
        explicit _Builder(BitHashBase<T> **addr) : _Builder<T, current - 1>(addr - 1) {
            *addr = reinterpret_cast<BitHashBase<T> *>(::operator new(sizeof(BitHash<T, current>)));
            emplace_construct<BitHash<T, current>>(reinterpret_cast<BitHash<T, current> *>(*addr));
        }

    };

    template<class T>
    struct _Builder<T, 0> {
        explicit _Builder(BitHashBase<T> **addr) {}
    };

}
#endif //DATA_STRUCTURE_FOR_LOVE_XFAST_HASH_HPP
