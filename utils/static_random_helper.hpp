//
// Created by schrodinger on 19-4-30.
//
#ifndef DATA_STRUCTURE_FOR_LOVE_RANDOM_HPP
#define DATA_STRUCTURE_FOR_LOVE_RANDOM_HPP

#include <random>

namespace data_structure::utils {
    template<class Int, Int min = 0, Int max = std::numeric_limits<Int>::max()>
    struct RandomIntGen {
        std::random_device seeding{};
        std::mt19937_64 eng{seeding()};
        std::uniform_int_distribution<Int> dist{min, max};
        Int operator()() noexcept { return dist(eng); }
    };

    template<class Int, Int min = 0, Int max = std::numeric_limits<Int>::max()>
    struct GeoIntGen {
        std::random_device seeding{};
        std::mt19937_64 eng{seeding()};
        std::geometric_distribution<Int> dist;

        Int operator()() noexcept { return dist(eng); }
    };

    template<typename Real>
    struct RandomRealGen {
        std::random_device seeding{};
        std::mt19937_64 eng{seeding()};
        std::uniform_real_distribution<Real> dist;

        explicit RandomRealGen(Real min = 0, Real max = 1) : dist(min, max) {}

        Real operator()() noexcept { return dist(eng); }
    };
}
#endif

