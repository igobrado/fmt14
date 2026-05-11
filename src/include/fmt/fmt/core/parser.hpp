#ifndef INCLUDE_FMT_CORE_PARSER_HPP
#define INCLUDE_FMT_CORE_PARSER_HPP

#include <fmt/core/format_string.hpp>
#include <fmt/core/types.hpp>
#include <fmt/fmt_internal/formatter.hpp>

namespace fmt {
namespace detail {

template<typename CharT>
class basic_parser
{
    using formatting_tokens = stl::fixed_buffer<fixed_format_string, k_max_formatting_tokens>;
    using fmt_string        = fmt::detail::max_fixed_basic_format_string<CharT>;

public:
    /**
     * @brief Constructs the basic_parser object.
     */
    constexpr basic_parser() = default;

    constexpr basic_parser(basic_parser const&) noexcept              = delete;
    constexpr basic_parser& operator=(basic_parser const&) & noexcept = delete;
    constexpr basic_parser& operator=(basic_parser&&) noexcept        = delete;
    constexpr basic_parser(basic_parser&) noexcept                    = delete;

    /**
     * @brief Parses the formatting context
     *
     * Converts the pair of angle brackets('{}') into the associated value given in
     * parameter pack 'a_values'
     *
     * @param a_format_string the formatting string
     * @param a_values        the value pairs to insert into the string
     *
     * @return the formatted string
     */
    template<typename... Args>
    constexpr auto parse(stl::detail::basic_string_view<CharT> const& a_format_string, Args&&... a_values)
        const noexcept
    {
        // TODO: It would be cleaner if fixed_format_string array would be called 'format_arguments' and be sent to this
        // fn as std::tuple<>, even more performant (?)
        formatting_tokens const stringified_replacements{to_format_string(std::forward<Args>(a_values))...};

        fmt::max_fixed_buffer_t<CharT> buffer{};
        CharT*                              dest      = buffer.data();
        std::size_t                         remaining = k_max_combined_string_size - 1;  // reserve for '\0'
        std::size_t                         index     = 0;

        CharT const* src = a_format_string.data();
        CharT const* end = src + a_format_string.size();

        while (src < end && remaining > 0)
        {
            // Fast path: literal text until next '{'
            if (*src != '{')
            {
                *dest++ = *src++;
                --remaining;
                continue;
            }

            // Check if this is a replacement token
            if ((src + 1 < end) && src[1] == '}' && index < stringified_replacements.size())
            {
                CharT const* replacement = stringified_replacements[index++].c_str();
                while (*replacement && remaining > 0)
                {
                    *dest++ = *replacement++;
                    --remaining;
                }
                src += 2;  // skip "{}"
            }
            else
            {
                // Just a '{' not followed by '}' → copy as literal
                *dest++ = *src++;
                --remaining;
            }
        }

        *dest = '\0';
        return fmt_string{std::move(buffer)};
    }
};
}  // namespace detail

using char_parser  = detail::basic_parser<char>;
using wchar_parser = detail::basic_parser<wchar_t>;

}  // namespace fmt

#endif  // INCLUDE_FMT_CORE_PARSE_CONTEXT_HPP
