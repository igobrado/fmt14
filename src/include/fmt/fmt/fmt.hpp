#ifndef INCLUDE_FMT_FMT_HPP
#define INCLUDE_FMT_FMT_HPP

#include <fmt/core/format_string.hpp>
#include <fmt/core/parser.hpp>

namespace fmt {

namespace detail {

/**
 * @brief Formats the @p a_format_args according to the @p a_format_string
 * and provides the buffer in a formatted version.
 *
 * @param a_format_string the format string describing how 'format_args' are supposed to be formatted in the result
 * @param a_format_args   the formatting arguments
 *
 * @return formatted string
 */
template<typename CharT, typename... Args>
constexpr auto basic_format(stl::detail::basic_string_view<CharT> const& a_format_string, Args&&... a_format_args)
    -> decltype(auto)
{
    return fmt::detail::basic_parser<CharT>{}.parse(a_format_string, std::forward<Args>(a_format_args)...);
}

}  // namespace detail

/**
 * @brief Formats the @p a_format_args according to the @p a_format_string
 * and provides the buffer in a formatted version.
 *
 * @param a_format_string the format string describing how 'format_args' are supposed to be formatted in the result
 * @param a_format_args   the formatting arguments
 *
 * @return formatted string
 */
template<typename... Args>
constexpr auto format(stl::string_view const& a_format_string, Args&&... a_format_args) -> decltype(auto)
{
    return detail::basic_format<char>(a_format_string, std::forward<Args>(a_format_args)...);
}

/**
 * @brief Formats the @p a_format_args according to the @p a_format_string
 * and provides the buffer in a formatted version.
 *
 * @param a_format_string the format string describing how 'format_args' are supposed to be formatted in the result
 * @param a_format_args   the formatting arguments
 *
 * @return formatted string
 */
template<typename... Args>
constexpr auto w_format(stl::wstring_view const& a_format_string, Args&&... a_format_args) -> decltype(auto)
{
    return detail::basic_format<wchar_t>(a_format_string, std::forward<Args>(a_format_args)...);
}

}  // namespace fmt

#endif  // INCLUDE_FMT_FMT_HPP
