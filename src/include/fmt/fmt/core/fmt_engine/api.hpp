#ifndef INCLUDE_FMT_CORE_FMT_ENGINE_API_HPP
#define INCLUDE_FMT_CORE_FMT_ENGINE_API_HPP

#include <fmt/core/fmt_engine.hpp>
#include <fmt/core/types.hpp>
#include <stl/floating_point.hpp>
#include <stl/traits.hpp>

namespace fmt {
namespace engine {
namespace api {

class converter
{
private:
    constexpr converter()                                      = default;
    constexpr converter(converter const&) noexcept             = delete;
    constexpr converter& operator=(converter const&) noexcept  = delete;
    constexpr converter& operator=(converter&& other) noexcept = delete;
    constexpr converter(converter&&) noexcept                  = default;

    /**
     * @brief Converts string value @p a_value into the fixed byte buffer.
     * @param a_value
     *
     * @return fixed byte buffer
     */
    template<typename CharT, typename R, stl::EnableIfString<R> = 0>
    constexpr fixed_buffer_t<CharT> operator()(R const& a_value) const noexcept
    {
        return c_str_to_array<CharT>(a_value);
    }

    /**
     * @brief Converts int value @p a_value into the fixed byte buffer.
     * @param a_value
     *
     * @return fixed byte buffer
     */
    template<typename CharT, typename R, stl::EnableIfInt<R> = 0>
    constexpr fixed_buffer_t<CharT> operator()(R const& a_value) const noexcept
    {
        fixed_buffer_t<CharT> arr{};
        char*                 out = arr.data();

        char* end = fmt::engine::format_integer(out, a_value);
        *end      = '\0';
        return arr;
    }

    /**
     * @brief Converts float value @p a_value into the fixed byte buffer.
     * @param a_value
     *
     * @return fixed byte buffer
     */
    template<typename CharT, typename R, stl::EnableIfFloat<R> = 0>
    constexpr fixed_buffer_t<CharT> operator()(R const& a_value) const noexcept
    {
        fixed_buffer_t<CharT> arr{};
        char*                 out = arr.data();

        auto const dec = stl::make_floating_point_number(a_value);
        char*      end = fmt::engine::format_decimal_fp<CharT, R>(out, dec);
        *end           = '\0';
        return arr;
    }

public:
    template<typename CharT, typename R>
    static constexpr fixed_buffer_t<CharT> convert(R const& a_value)
    {
        return fmt::engine::api::converter{}.template operator()<CharT>(a_value);
    }
};

}  // namespace api
}  // namespace engine
}  // namespace fmt

#endif  //  INCLUDE_FMT_CORE_FMT_ENGINE_API_HPP
