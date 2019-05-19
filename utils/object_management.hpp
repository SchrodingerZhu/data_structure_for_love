//
// Created by schrodinger on 2/16/19.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_OBJECT_MANAGEMENT_HPP
#define DATA_STRUCTURE_FOR_LOVE_OBJECT_MANAGEMENT_HPP

#include <utility>

namespace data_structure::utils {

        template <typename T, typename ... Args>
        inline void emplace_construct(T* t, Args&& ...args) noexcept {
            ::new (t) T(std::forward<Args>(args)...);
        }

        template <typename T>
        inline void destroy_at(T* t) noexcept {t -> ~T();}

    }
#endif //DATA_STRUCTURE_FOR_LOVE_OBJECT_MANAGEMENT_HPP
