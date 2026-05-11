#ifndef INCLUDE_STL_TRAITS_HPP
#define INCLUDE_STL_TRAITS_HPP

#include <string>
#include <type_traits>
#include <stl/string_view.hpp>

#if __cplusplus < 201703L

namespace std {
template<typename T>
constexpr bool is_integral_v = std::is_integral<T>::value;
// C++14 version of std::disjunction
template<typename...>
struct disjunction : std::false_type
{
};

template<typename B1>
struct disjunction<B1> : B1
{
};

template<typename B1, typename... Bn>
struct disjunction<B1, Bn...> : std::conditional<B1::value, B1, disjunction<Bn...>>::type
{
};
}  // namespace std
#endif

namespace stl {

/**
 * @brief Checks whether a given @tparam T is string like type.
 *
 * Type is considered as a string in case that is char*, char const*,
 * std::string, or string_view.
 */
template<typename T>
struct is_string
: public std::disjunction<
      std::is_same<char*, std::decay_t<T>>,
      std::is_same<const char*, std::decay_t<T>>,
      std::is_same<std::string, std::decay_t<T>>,
      std::is_same<stl::string_view, std::decay_t<T>>>
{
};

/**
 * @brief SFINAE helper for checking whether type is a float.
 */
template<typename T>
using EnableIfFloat = typename std::enable_if<std::is_floating_point<typename std::decay_t<T>>::value, int>::type;

/**
 * @brief SFINAE helper for checking whether type is a integer.
 */
template<typename T>
using EnableIfInt = typename std::enable_if<std::is_integral<typename std::decay_t<T>>::value, int>::type;

/**
 * @brief SFINAE helper for checking whether type is a string.
 */
template<typename T>
using EnableIfString = typename std::enable_if<is_string<T>::value, int>::type;

}  // namespace stl

#endif  // INCLUDE_STL_TRAITS_HPP
