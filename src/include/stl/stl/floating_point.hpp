#ifndef INCLUDE_STL_FLOATING_POINT_HPP
#define INCLUDE_STL_FLOATING_POINT_HPP

#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <stl/traits.hpp>

namespace stl {

/**
 * @brief Repesentation of the floating point number
 */
template<typename Float>
struct floating_point
{
    using significand_type =
        typename std::make_unsigned<typename std::conditional<sizeof(Float) <= 4, uint32_t, uint64_t>::type>::type;

    /**
     * @brief Constructs the default floating point object
     */
    constexpr floating_point() = default;

    /**
     * @brief Constructs the floating point number with given parameters
     *
     * @param sig
     * @param exp
     */
    constexpr floating_point(significand_type sig, int exp)
    : m_significand(sig)
    , m_exponent(exp)
    {
    }

    significand_type m_significand;  ///< The integer significand
    int              m_exponent;     ///< The decimal exponent
};

/**
 * @brief Constructs the 'floating_point' data structure from a @tparam Float
 *
 * @param a_number floating point number
 *
 * @return 'floating_point' data structure.
 */
template<typename Float, stl::EnableIfFloat<Float> = 0>
constexpr floating_point<Float> make_floating_point_number(Float a_number) noexcept
{
    if (a_number == Float(0))
    {
        return floating_point<Float>{0, 0};
    }

    // Handle sign
    bool const negative = a_number < 0;

    if (negative)
    {
        a_number = -a_number;
    }

    int exp = 0;
    while (a_number >= Float(10))
    {
        a_number /= 10;
        ++exp;
    }
    while (a_number < Float(1))
    {
        a_number *= 10;
        --exp;
    }

    // Compute significand as integer
    // TODO:  How much digits one wants?!

    auto significand = static_cast<typename floating_point<Float>::significand_type>(a_number * 1000000 + 0.5);
    return floating_point<Float>{negative ? -significand : significand, exp - 6};
}

}  // namespace stl

#endif  // INCLUDE_STL_FLOATING_POINT_HPP
