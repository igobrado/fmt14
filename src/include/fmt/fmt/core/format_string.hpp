#ifndef INCLUDE_FMT_CORE_FORMAT_STRING_HPP
#define INCLUDE_FMT_CORE_FORMAT_STRING_HPP

#include <fmt/core/fmt_engine/api.hpp>
#include <fmt/core/types.hpp>
#include <stl/fixed_buffer.hpp>
#include <stl/string_view.hpp>

namespace fmt {

namespace detail {
/**
 * @brief The basic_format_string class
 */
template<typename CharT, std::size_t N = k_max_single_string_size>
class basic_format_string
{
    /**
     * @brief Type alias for the underlying type
     */
    using char_buffer_t = stl::fixed_buffer<CharT, N>;

public:
    constexpr basic_format_string() = default;

    /**
     * @brief Converts the @tparam T into the formatted string
     *
     * @param a_value the value to format
     */
    template<typename T, typename std::enable_if_t<!std::is_same<T, char_buffer_t>::value, int> = 0>
    constexpr basic_format_string(T const& a_value) noexcept
    : m_buffer{fmt::engine::api::converter::convert<CharT>(a_value)}
    {
    }

    /**
     * @brief Creates the formatted string from a buffer
     *
     * @param a_buffer
     */
    template<typename T, typename std::enable_if_t<std::is_same<T, char_buffer_t>::value, int> = 0>
    constexpr basic_format_string(T&& a_buffer)
    : m_buffer{std::move(a_buffer)}
    {
    }

    /**
     * @brief Constructs the format string from given variadic arguments
     *
     * @param a_values
     */
    template<typename... Args>
    constexpr basic_format_string(Args&&... a_values) noexcept
    : m_buffer{make_buffer(std::forward<Args>(a_values)...)}
    {
    }

    constexpr basic_format_string(basic_format_string const& other) noexcept
    : m_buffer{other.m_buffer}
    {
    }
    constexpr basic_format_string& operator=(basic_format_string const&) noexcept  = default;
    constexpr basic_format_string& operator=(basic_format_string&& other) noexcept = default;
    constexpr basic_format_string(basic_format_string&&) noexcept                  = default;

    /**
     * @brief Explicit conversion to string view
     */
    constexpr auto to_string_view() const
    {
        return stl::detail::basic_string_view<CharT>{c_str()};
    }

    /**
     * @brief Implicit conversion operator from fixed_buffer to string_view
     */
    constexpr operator stl::detail::basic_string_view<CharT>() const
    {
        return to_string_view();
    }

    /**
     * @brief Implicit conversion operator from fixed_buffer to CharT const*
     */
    constexpr operator CharT const*() const
    {
        return c_str();
    }

    /**
     * @brief Retreives the cstring to the underlying buffer
     *
     * @return the c_string
     */
    constexpr CharT const* c_str() const noexcept
    {
        return m_buffer.get();
    }

private:
    /**
     * @brief Builds the buffer from the variadic arguments
     *
     * @param args
     * @return
     */
    template<typename... Args>
    constexpr char_buffer_t make_buffer(Args&&... args) noexcept
    {
        char_buffer_t buf{};    // zero-initialized
        std::size_t   pos = 0;  // current write cursor
        append_all(buf, pos, std::forward<Args>(args)...);

        // ensure null-termination if space allows
        if (pos < buf.size())
        {
            buf[pos] = CharT{};
        }
        else if (buf.size() > 0)
        {
            buf[buf.size() - 1] = CharT{};  // truncate, keep NUL
        }
        return buf;
    }

    // clang-format off
    /**
     * @brief recursion base
     */
    constexpr void append_all(char_buffer_t&, std::size_t&) noexcept {}
    // clang-format on

    // recursion step
    template<typename T, typename... Rest>
    constexpr void append_all(char_buffer_t& a_destination, std::size_t& pos, T&& a_value, Rest&&... a_rest) noexcept
    {
        append_one(a_destination, pos, std::forward<T>(a_value));
        append_all(a_destination, pos, std::forward<Rest>(a_rest)...);
    }

    /**
     * @brief Appends one element into the buffer
     *
     * @param a_destination destination buffer
     * @param a_position    the currenct position
     * @param a_value       the value to format
     */
    template<typename T>
    constexpr void append_one(char_buffer_t& a_destination, std::size_t& a_position, T&& a_value) noexcept
    {
        // Format this argument into a temporary fixed_buffer_t
        auto const tmp = fmt::engine::api::converter::convert<CharT>(std::forward<T>(a_value));

        // Copy characters until null terminator, leaving room for final '\0'
        auto const* src = tmp.get();  // your fixed_buffer has get()
        while (a_position + 1 < a_destination.size() && *src != CharT{})
        {
            a_destination[a_position++] = *src++;
        }
    }

    char_buffer_t m_buffer;
};

template<typename CharT>
using fixed_basic_format_string = detail::basic_format_string<CharT, k_max_single_string_size>;

template<typename CharT>
using max_fixed_basic_format_string = detail::basic_format_string<CharT, k_max_combined_string_size>;

}  // namespace detail

using fixed_format_string     = detail::fixed_basic_format_string<char>;
using max_fixed_format_string = detail::fixed_basic_format_string<char>;

using fixed_format_wstring     = detail::fixed_basic_format_string<wchar_t>;
using max_fixed_format_wstring = detail::fixed_basic_format_string<wchar_t>;

}  // namespace fmt

#endif  // INCLUDE_FMT_CORE_FORMAT_STRING_HPP
