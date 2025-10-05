#pragma once
#include <random>
#include <type_traits>

namespace tur
{
    template <typename T, typename Generator = std::mt19937_64>
    inline T generate_random(const T& lowerBound = std::numeric_limits<T>::min(),
                             const T& upperBound = std::numeric_limits<T>::max())
    {
        Generator generator{std::random_device{}()};

        if constexpr (std::is_integral<T>::value)
        {
            std::uniform_int_distribution<T> distribution(lowerBound, upperBound);
            return distribution(generator);
        }

        else if constexpr (std::is_floating_point<T>::value)
        {
            std::uniform_real_distribution<T> distribution(lowerBound, upperBound);
            return distribution(generator);
        }

        return T{};
    }
}