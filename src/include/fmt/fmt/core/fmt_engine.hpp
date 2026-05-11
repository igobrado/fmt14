#ifndef FMT_ENGINE_HPP
#define FMT_ENGINE_HPP

#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <fmt/core/types.hpp>
#include <stl/floating_point.hpp>
#include <stl/traits.hpp>

// TODO: Make this object oriented and documentation

namespace fmt {
namespace engine {

struct digits2_table
{
public:
    static constexpr char const* get_chars(std::size_t a_value)
    {
        return &m_array[a_value * 2];
    }

    alignas(2) static constexpr std::array<char, 201> m_array{
        "0001020304050607080910111213141516171819"
        "2021222324252627282930313233343536373839"
        "4041424344454647484950515253545556575859"
        "6061626364656667686970717273747576777879"
        "8081828384858687888990919293949596979899"};
};
alignas(2) constexpr std::array<char, 201> digits2_table::m_array;

template<std::size_t N>
constexpr void constexpr_memcpy(char* a_dest, char const* a_src)
{
    for (std::size_t i = 0; i < N; ++i)
    {
        a_dest[i] = a_src[i];
    }
}

template<typename Char>
constexpr inline void write2digits(Char* a_out, unsigned a_value)
{
    constexpr_memcpy<2>(a_out, digits2_table::get_chars(a_value));
}

// ---------------------------------------------------------
// Fast digit counts
//  - 32-bit: CLZ trick (Lemire/Kendall Willets)
//  - 64-bit: branchy thresholds (no loops)
// ---------------------------------------------------------
constexpr inline int count_digits32(std::uint32_t a_number)
{
#define INC(T) (((sizeof(#T) - 1ull) << 32) - (T))
    constexpr uint64_t table[] = {INC(0),          INC(0),          INC(0),     // 8
                                  INC(10),         INC(10),         INC(10),    // 64
                                  INC(100),        INC(100),        INC(100),   // 512
                                  INC(1000),       INC(1000),       INC(1000),  // 4096
                                  INC(10000),      INC(10000),      INC(10000),      INC(100000),     INC(100000),
                                  INC(100000),     INC(1000000),    INC(1000000),    INC(1000000),    INC(10000000),
                                  INC(10000000),   INC(10000000),   INC(100000000),  INC(100000000),  INC(100000000),
                                  INC(1000000000), INC(1000000000), INC(1000000000), INC(1000000000), INC(1000000000)};
#undef INC
#if defined(__GNUG__) || defined(__clang__)
    auto inc = table[__builtin_clz(a_number | 1) ^ 31];
#else
    // Portable fallback (slower): normalize to get "pseudo-CLZ"
    int      lz = 0;
    uint32_t t  = n | 1u;
    while ((t & 0x80000000u) == 0u)
    {
        t <<= 1;
        ++lz;
    }
    auto inc = table[lz ^ 31];
#endif
    return static_cast<int>((static_cast<std::uint64_t>(a_number) + inc) >> 32);
}

inline constexpr int count_digits64(std::uint64_t a_number)
{
    // Branchy, no division, very fast in practice.
    if (a_number >= 10000000000000000000ull)
        return 20;
    if (a_number >= 1000000000000000000ull)
        return 19;
    if (a_number >= 100000000000000000ull)
        return 18;
    if (a_number >= 10000000000000000ull)
        return 17;
    if (a_number >= 1000000000000000ull)
        return 16;
    if (a_number >= 100000000000000ull)
        return 15;
    if (a_number >= 10000000000000ull)
        return 14;
    if (a_number >= 1000000000000ull)
        return 13;
    if (a_number >= 100000000000ull)
        return 12;
    if (a_number >= 10000000000ull)
        return 11;
    if (a_number >= 1000000000ull)
        return 10;
    if (a_number >= 100000000ull)
        return 9;
    if (a_number >= 10000000ull)
        return 8;
    if (a_number >= 1000000ull)
        return 7;
    if (a_number >= 100000ull)
        return 6;
    if (a_number >= 10000ull)
        return 5;
    if (a_number >= 1000ull)
        return 4;
    if (a_number >= 100ull)
        return 3;
    if (a_number >= 10ull)
        return 2;
    return 1;
}

// ---------------------------------------------------------
// Core decimal writer: write from the end toward the front
// Returns (out + size) — the end pointer.
// PRECONDITION: size == exact number of digits in 'value'.
// ---------------------------------------------------------
template<typename Char, typename UInt>
inline constexpr Char* format_decimal(Char* p_out, UInt a_value, int a_size)
{
    // We maintain an index 'n' counting remaining slots from the front.
    // We only ever compute out + n where 0 <= n <= size.
    int n = a_size;

    while (a_value >= 100)
    {
        unsigned two = static_cast<unsigned>(a_value % 100);
        a_value /= 100;
        n -= 2;
        assert(n >= 0 && n + 2 <= a_size);
        write2digits(p_out + n, two);
    }

    if (a_value >= 10)
    {
        n -= 2;
        assert(n >= 0 && n + 2 <= a_size);
        write2digits(p_out + n, static_cast<unsigned>(a_value));
    }
    else
    {
        --n;
        assert(n >= 0 && n < a_size);
        p_out[n] = static_cast<Char>('0' + static_cast<unsigned>(a_value));
    }

    // All digits must have been filled.
    assert(n == 0);
    return p_out + a_size;
}

// ---------------------------------------------------------
// Public API: format integer to buffer, return end pointer.
// Works for any integral T, no if-constexpr.
// ---------------------------------------------------------
template<typename Char, typename T, stl::EnableIfInt<T> = 0>
inline constexpr Char* format_integer(Char* p_out, T p_value)
{
    using unsigned_type_t = typename std::make_unsigned<T>::type;

    unsigned_type_t u_value  = static_cast<unsigned_type_t>(p_value);
    bool            negative = false;

    if (std::is_signed<T>::value)
    {  // runtime type check (OK in C++14/17)
        if (p_value < 0)
        {
            negative = true;
            u_value  = ~u_value + 1;  // two's complement abs
        }
    }

    // Compute digit count based on width of U
    int digits = 0;
    if (sizeof(unsigned_type_t) <= 4)
    {
        digits = count_digits32(static_cast<std::uint32_t>(u_value));
    }
    else
    {
        digits = count_digits64(static_cast<std::uint64_t>(u_value));
    }

    // Optional leading '-'
    Char* p = p_out;
    if (negative)
    {
        *p++ = static_cast<Char>('-');
    }

    // Write the digits right after the sign; return end pointer.
    return format_decimal(p, u_value, digits);
}

template<typename Char, typename T>
constexpr Char* format_decimal_fp(Char* a_out, stl::floating_point<T> const& a_dec)
{
    Char* ptr = a_out;

    // Optional negative sign
    if (a_dec.m_significand < 0)
    {
        *ptr++ = '-';
    }

    // Convert absolute significand to char array
    using uint_t   = typename std::make_unsigned<decltype(a_dec.m_significand)>::type;
    uint_t abs_sig = static_cast<uint_t>(a_dec.m_significand);

    // Count digits
    int digits = (sizeof(uint_t) <= 4) ? engine::count_digits32(abs_sig) : engine::count_digits64(abs_sig);

    // Write digits to buffer (end-aligned)
    Char temp[20] = {};  // max digits for 64-bit
    engine::format_decimal(temp, abs_sig, digits);

    // Now insert decimal point based on exponent
    int decimal_pos = digits + a_dec.m_exponent;  // exponent is base-10

    if (decimal_pos <= 0)
    {
        // 0.00..digits
        *ptr++ = '0';
        *ptr++ = '.';
        for (int i = 0; i < -decimal_pos; ++i)
        {
            *ptr++ = '0';
        }

        for (int i = 0; i < digits; ++i)
        {
            *ptr++ = temp[i];
        }
    }
    else
    {
        // digits before decimal point
        for (int i = 0; i < decimal_pos && i < digits; ++i)
        {
            *ptr++ = temp[i];
        }

        if (decimal_pos < digits)
        {
            *ptr++ = '.';
            for (int i = decimal_pos; i < digits; ++i)
            {
                *ptr++ = temp[i];
            }
        }
    }

    *ptr = '\0';
    return ptr;
}

template<typename CharT>
static constexpr fixed_buffer_t<CharT> c_str_to_array(stl::detail::basic_string_view<CharT> const& a_str)
{
    fixed_buffer_t<CharT> arr{};
    CharT*                out = arr.data();
    CharT const*          src = a_str.data();

    for (std::size_t i = 0; i < fixed_buffer_t<CharT>::max_size() - 1 && *src; ++i)
    {
        *out++ = *src++;
    }
    *out = '\0';
    return arr;
}

}  // namespace engine
}  // namespace fmt

#endif  // FMT_ENGINE_HPP
