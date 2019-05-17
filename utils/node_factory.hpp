//
// Created by schrodinger on 19-5-15.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_NODE_FACTORY_HPP
#define DATA_STRUCTURE_FOR_LOVE_NODE_FACTORY_HPP

#include <object_pool.hpp>

namespace data_structure::utils {
    struct NodeFactory {
        constexpr static bool meldable = false;
    };

    template<class Node>
    class TrivialFactory : NodeFactory {
    public:
        constexpr static bool meldable = true;

        template<class ...Args>
        [[nodiscard]] Node *construct(Args &&... args) {
            return new Node(std::forward<Args>(args)...);
        }

        void destroy(Node *t) {
            delete t;
        }

        void absorb(TrivialFactory &that) {/*Nothing*/}
    };

    template<class Node, std::size_t N = 500>
    class PoolFactory : NodeFactory {
        ObjectPool<Node, N> pool;
    public:
        constexpr static bool meldable = true;

        template<class ...Args>
        [[nodiscard]] Node *construct(Args &&... args) {
            return pool.construct_raw(std::forward<Args>(args)...);
        }

        void destroy(Node *t) {
            pool.recycle(t);
        }

        void absorb(PoolFactory &that) {
            pool.absorb(that);
        }
    };


}

#endif //DATA_STRUCTURE_FOR_LOVE_NODE_FACTORY_HPP
