//
// Created by schrodinger on 19-5-20.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_COMPARE_HPP
#define DATA_STRUCTURE_FOR_LOVE_COMPARE_HPP
namespace data_structure::utils {
    enum Relation {
        Eq, Less, Greater
    };

    template<class T>
    struct DefaultCompare {
        constexpr Relation operator()(const T &a, const T &b) const noexcept {
            if (a == b) return utils::Eq;
            else if (a < b) return utils::Less;
            else return utils::Greater;
        }
    };
}
#endif //DATA_STRUCTURE_FOR_LOVE_COMPARE_HPP
