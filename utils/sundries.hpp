//
// Created by schrodinger on 2/22/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_SUNDRIES_HPP
#define DATA_STRUCTURE_FOR_LOVE_SUNDRIES_HPP
#include <cstddef>

namespace data_structure {
    namespace utils {

        template <class Child, class Parent, typename = std::enable_if_t<std::is_base_of_v<Parent, Child>>>
        Child* as_pointer_of(Parent* t){
            return dynamic_cast<Child *>(t);
        }

        template<typename... A>
        struct Size {
            constexpr static size_t size = 0;
        };
        template<typename Head, typename... Tail>
        struct Size<Head, Tail...> {
            constexpr static size_t size = sizeof(typename Head::type) + Size<Tail...>::size;
        };

        template<typename... A>
        struct Count {
            constexpr static size_t count = 0;
        };
        template<typename Head, typename... Tail>
        struct Count<Head, Tail...> {
            constexpr static size_t count = 1 + Count<Tail...>::count;
        };

        template <size_t N, typename T, typename... Tn>
        struct TypeHelper{
            using type = typename TypeHelper<N - 1, Tn...>::type;
        };

        template <typename T, typename... Tn>
        struct TypeHelper<0, T, Tn...>{
            using type = T;
        };

    }
}

#endif //DATA_STRUCTURE_FOR_LOVE_SUNDRIES_HPP
