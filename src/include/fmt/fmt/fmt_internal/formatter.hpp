#ifndef INCLUDE_FMT_FMT_INTERNAL_FORMATTER_HPP
#define INCLUDE_FMT_FMT_INTERNAL_FORMATTER_HPP

#include <type_traits>
#include <fmt/core/format_string.hpp>

namespace fmt {

/**
 * @brief Formats the @tparam TypeToFormat into the a formatted string
 *
 * @warning not supported and to be implemented, user defined types formatting.
 */
template<typename TypeToFormat, typename Enable = void>
struct formatter
{
    constexpr fixed_format_string operator()(TypeToFormat)
    {
        static_assert(sizeof(TypeToFormat) == 0, "Specialization required for Type T.");
        return fixed_format_string{"invalid"};
    }
};

template<typename T>
struct formatter<T, std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
{
    constexpr fixed_format_string operator()(T const& a_value)
    {
        return fixed_format_string{a_value};
    }
};

template<typename T>
struct formatter<T, std::enable_if_t<stl::is_string<T>::value>>
{
    constexpr fixed_format_string operator()(T const& a_value)
    {
        return fixed_format_string{a_value};
    }
};

template<typename T>
struct formatter<T, std::enable_if_t<std::is_floating_point<std::decay_t<T>>::value>>
{
    constexpr fixed_format_string operator()(T const& a_value)
    {
        return fixed_format_string{a_value};
    }
};

template<typename TypeToFormat>
constexpr static fixed_format_string to_format_string(TypeToFormat&& a_value) noexcept
{
    // typename std::decay_t is used to specialization of user defined types easier
    return formatter<typename std::decay_t<TypeToFormat>>{}(std::forward<TypeToFormat>(a_value));
}

}  // namespace fmt

#endif  // INCLUDE_FMT_FMT_INTERNAL_FORMATTER_HPP
