#ifndef INCLUDE_FMT_TYPES_HPP
#define INCLUDE_FMT_TYPES_HPP

#include <stl/fixed_buffer.hpp>

namespace fmt {

/**
 * @brief Configurable number that determines what is the biggest number of formating
 * arguments.
 *
 * e.g.
 * If 6 is configured largest number of formatting tokens can be 6;
 * fmt("{}, {}, {}, {}, {}, {}, {}", 1,2,3,4,5,6,7); will yield compile error.
 */
static constexpr std::uint8_t k_max_formatting_tokens = 4u;

/**
 * @brief Configurable number that determines what is the biggest number of single formatted string.
 *
 * e.g.
 * If 10 is configured, single format string must not be bigger than 10 characters.
 * fmt{"{}", "hello_my_name_is_cetitec_and_this_wont_work") -  your string will be "hello_my_n".
 */
static constexpr std::uint8_t k_max_single_string_size = 128u;

/**
 * @brief Configurable number that determines what can be the biggest (combined) string formatted.
 *
 * e.g.
 * If 20 is configured, sum of all formatting arguments ( including format ), must be less than 20.
 */
static constexpr std::uint16_t k_max_combined_string_size = 512u;

/**
 * @brief Type alias for the fixed buffer.
 */
template<typename CharT>
using fixed_buffer_t = typename stl::fixed_buffer<CharT, k_max_single_string_size>;

/**
 * @brief Type alias for fixed buffer of max size.
 */
template<typename CharT>
using max_fixed_buffer_t = typename stl::fixed_buffer<CharT, k_max_combined_string_size>;

}  // namespace fmt

#endif  // INCLUDE_FMT_TYPES_HPP
