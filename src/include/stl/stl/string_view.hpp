#ifndef INCLUDE_STL_STRING_VIEW_HPP
#define INCLUDE_STL_STRING_VIEW_HPP

#ifdef USE_NATIVE_STRING_VIEW
#    include <string_view>
#endif

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <stl/fixed_buffer.hpp>

namespace stl {
namespace detail {

/**
 * @brief Implementation of the string_view like interface.
 */
template<typename CharT, typename TraitsT = std::char_traits<CharT>>
class basic_string_view final
{
    static constexpr std::size_t constexpr_strlen(CharT const* a_str) noexcept
    {
        std::size_t len = 0;
        for (; *a_str != CharT{}; ++a_str)
        {
            ++len;
        }
        return len;
    }

    static constexpr int compare(CharT const* a_lhs, CharT const* a_rhs, std::size_t a_size) noexcept
    {
        for (std::size_t i = 0; i < a_size; ++i)
        {
            if (a_lhs[i] < a_rhs[i])
            {
                return -1;
            }

            if (a_lhs[i] > a_rhs[i])
            {
                return 1;
            }
        }
        return 0;
    }

public:
    using traits_type            = TraitsT;
    using value_type             = CharT;
    using pointer                = value_type*;
    using const_pointer          = value_type const*;
    using reference              = value_type&;
    using const_reference        = value_type const&;
    using const_iterator         = const_pointer;
    using iterator               = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator       = const_reverse_iterator;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;

    static constexpr size_type const npos = static_cast<size_type>(-1);

    constexpr basic_string_view() noexcept
    : m_data(nullptr)
    , m_size(0)
    {
    }

    constexpr basic_string_view(basic_string_view const&) noexcept              = default;
    constexpr basic_string_view& operator=(basic_string_view const&) & noexcept = default;
    constexpr basic_string_view(const_pointer s)
    : basic_string_view(s, constexpr_strlen(s))
    {
    }

    constexpr basic_string_view(const_pointer s, size_type size)
    : m_data(s)
    , m_size(size)
    {
    }

    basic_string_view(std::string const& s) noexcept
    : m_data(s.data())
    , m_size(s.size())
    {
    }

    constexpr basic_string_view(std::nullptr_t)              = delete;
    constexpr basic_string_view(std::nullptr_t, std::size_t) = delete;

    constexpr operator CharT const*() const noexcept
    {
        return cbegin();
    }

    constexpr const_iterator begin() const noexcept
    {
        return cbegin();
    }

    constexpr const_iterator end() const noexcept
    {
        return cend();
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return &data()[0];
    }

    constexpr const_iterator cend() const noexcept
    {
        return &data()[m_size];
    }

    // should be constexpr, but not possible before C++17 because std::reverse_iterator isn't
    const_reverse_iterator rbegin() const noexcept
    {
        return crbegin();
    }

    // should be constexpr, but not possible before C++17 because std::reverse_iterator isn't
    const_reverse_iterator rend() const noexcept
    {
        return crend();
    }

    // should be constexpr, but not possible before C++17 because std::reverse_iterator isn't
    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    // should be constexpr, but not possible before C++17 because std::reverse_iterator isn't
    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr size_type size() const noexcept
    {
        return m_size;
    }

    constexpr size_type length() const noexcept
    {
        return m_size;
    }

    constexpr size_type max_size() const noexcept
    {
        return (npos - sizeof(size_type) - sizeof(void*)) / sizeof(value_type) / 2;
    }

    constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    constexpr const_reference operator[](size_type pos) const
    {
        return data()[pos];
    }

    constexpr const_reference at(size_type pos) const
    {
        if (pos >= size())
        {
            throw std::out_of_range("index to at() is out of range");
        }
        return data()[pos];
    }

    constexpr const_reference front() const
    {
        return data()[0];
    }

    constexpr const_reference back() const
    {
        return data()[size() - 1];
    }

    constexpr const_pointer data() const noexcept
    {
        return m_data;
    }

    constexpr void remove_prefix(size_type n)
    {
        m_data += n;
        m_size -= n;
    }

    constexpr void remove_suffix(size_type n)
    {
        m_size -= n;
    }

    constexpr void swap(basic_string_view& sv) noexcept
    {
        const_pointer const tmp_data = m_data;
        m_data                       = sv.m_data;
        sv.m_data                    = tmp_data;

        size_type const tmp_size = m_size;
        m_size                   = sv.m_size;
        sv.m_size                = tmp_size;
    }

    size_type copy(CharT* s, size_type n, size_type pos = 0U) const
    {
        if (pos > size())
        {
            throw std::out_of_range("invalid index");
        }

        size_type const rlen = std::min(n, size() - pos);
        std::copy(data() + pos, data() + pos + rlen, s);
        return rlen;
    }

    constexpr basic_string_view substr(size_type pos = 0U, size_type n = npos) const
    {
        if (pos > size())
        {
            throw std::out_of_range("invalid index");
        }

        size_type const rlen = std::min(n, size() - pos);
        return basic_string_view(data() + pos, rlen);
    }

    constexpr int compare(basic_string_view sv) const noexcept
    {
        int const ret = compare(data(), sv.data(), std::min(size(), sv.size()));
        if (ret == 0)
        {
            return (size() < sv.size()) ? -1 : (size() > sv.size());
        }
        return ret;
    }

    constexpr int compare(size_type pos1, size_type n1, basic_string_view sv) const
    {
        return substr(pos1, n1).compare(sv);
    }

    constexpr int compare(size_type pos1, size_type n1, basic_string_view sv, size_type pos2, size_type n2) const
    {
        return substr(pos1, n1).compare(sv.substr(pos2, n2));
    }

    constexpr int compare(const_pointer s) const
    {
        return compare(basic_string_view(s));
    }

    constexpr int compare(size_type pos1, size_type n1, const_pointer s) const
    {
        return substr(pos1, n1).compare(basic_string_view(s));
    }

    constexpr int compare(size_type pos1, size_type n1, const_pointer s, size_type n2) const
    {
        return substr(pos1, n1).compare(basic_string_view(s, n2));
    }

    constexpr size_type find(basic_string_view sv, size_type pos = 0U) const noexcept;

    constexpr size_type find(CharT c, size_type pos = 0U) const noexcept;

    constexpr size_type find(CharT const* s, size_type pos, size_type n) const
    {
        return find(basic_string_view(s, n), pos);
    }

    constexpr size_type find(CharT const* s, size_type pos = 0U) const
    {
        return find(basic_string_view(s), pos);
    }

    constexpr size_type rfind(basic_string_view sv, size_type pos = npos) const noexcept;

    constexpr size_type rfind(CharT c, size_type pos = npos) const noexcept;

    constexpr size_type rfind(CharT const* s, size_type pos, size_type n) const
    {
        return rfind(basic_string_view(s, n), pos);
    }

    constexpr size_type rfind(CharT const* s, size_type pos = npos) const
    {
        return rfind(basic_string_view(s), pos);
    }

    constexpr size_type find_first_of(basic_string_view sv, size_type pos = 0U) const noexcept;

    constexpr size_type find_first_of(CharT ch, size_type pos = 0U) const noexcept
    {
        return find(ch, pos);
    }

    constexpr size_type find_first_of(CharT const* s, size_type pos, size_type n) const
    {
        return find_first_of(basic_string_view(s, n), pos);
    }

    constexpr size_type find_first_of(CharT const* s, size_type pos = 0U) const
    {
        return find_first_of(basic_string_view(s), pos);
    }

    constexpr size_type find_last_of(basic_string_view sv, size_type pos = npos) const noexcept;

    constexpr size_type find_last_of(CharT c, size_type pos = npos) const noexcept
    {
        return rfind(c, pos);
    }

    constexpr size_type find_last_of(CharT const* s, size_type pos, size_type n) const
    {
        return find_last_of(basic_string_view(s, n), pos);
    }

    constexpr size_type find_last_of(CharT const* s, size_type pos = npos) const
    {
        return find_last_of(basic_string_view(s), pos);
    }

    constexpr size_type find_first_not_of(basic_string_view sv, size_type pos = 0U) const noexcept;

    constexpr size_type find_first_not_of(CharT c, size_type pos = 0U) const noexcept;

    constexpr size_type find_first_not_of(CharT const* s, size_type pos, size_type n) const
    {
        return find_first_not_of(basic_string_view(s, n), pos);
    }

    constexpr size_type find_first_not_of(CharT const* s, size_type pos = 0U) const
    {
        return find_first_not_of(basic_string_view(s), pos);
    }

    constexpr size_type find_last_not_of(basic_string_view sv, size_type pos = npos) const noexcept;

    constexpr size_type find_last_not_of(CharT c, size_type pos = npos) const noexcept;

    constexpr size_type find_last_not_of(CharT const* s, size_type pos, size_type n) const
    {
        return find_last_not_of(basic_string_view(s, n), pos);
    }

    constexpr size_type find_last_not_of(CharT const* s, size_type pos = npos) const
    {
        return find_last_not_of(basic_string_view(s), pos);
    }

    // C++20 additions

    constexpr bool starts_with(basic_string_view sv) const noexcept
    {
        return substr(0, sv.size()) == sv;
    }

    constexpr bool starts_with(CharT c) const noexcept
    {
        return !empty() && (front() == c);
    }

    constexpr bool starts_with(CharT const* s) const
    {
        return starts_with(basic_string_view(s));
    }

    constexpr bool ends_with(basic_string_view sv) const noexcept
    {
        if (size() < sv.size())
        {
            return false;
        }

        return substr(size() - sv.size()) == sv;
    }

    constexpr bool ends_with(CharT c) const noexcept
    {
        return !empty() && (back() == c);
    }

    constexpr bool ends_with(CharT const* s) const
    {
        return ends_with(basic_string_view(s));
    }

    // C++23 additions

    constexpr bool contains(basic_string_view sv) const noexcept
    {
        return find(sv) != npos;
    }

    constexpr bool contains(CharT c) const noexcept
    {
        return find(c) != npos;
    }

    constexpr bool contains(CharT const* s) const
    {
        return contains(basic_string_view(s));
    }

private:
    const_pointer m_data;
    size_type     m_size;
};

template<typename CharT, typename TraitsT>
constexpr auto basic_string_view<CharT, TraitsT>::find(basic_string_view<CharT, TraitsT> sv, size_type pos)
    const noexcept -> size_type
{
    if (sv.size() == 0)
    {
        return (pos <= size()) ? pos : npos;
    }

    if (sv.size() <= size())
    {
        for (std::size_t i = pos; i <= (size() - sv.size()); ++i)
        {
            if (TraitsT::compare(data() + i, sv.data(), sv.size()) == 0)
            {
                return i;
            }
        }
    }
    return npos;
}

template<typename CharT, typename TraitsT>
constexpr auto basic_string_view<CharT, TraitsT>::find(CharT c, size_type pos) const noexcept -> size_type
{
    if (pos >= size())
    {
        return npos;
    }
    CharT const* start = data() + pos;
    CharT const* it    = TraitsT::find(start, size() - pos, c);
    if (it != nullptr)
    {
        return static_cast<size_type>(it - data());
    }
    return npos;
}

template<typename CharT, typename TraitsT>
constexpr auto basic_string_view<CharT, TraitsT>::rfind(basic_string_view sv, size_type pos) const noexcept -> size_type
{
    if (sv.size() > size())
    {
        return npos;
    }

    size_type n = sv.size();
    pos         = std::min(static_cast<size_type>(size() - n), pos);
    do
    {
        if (TraitsT::compare(data() + pos, sv.data(), n) == 0)
        {
            return pos;
        }
    } while (pos-- > 0);
    return npos;
}

template<typename CharT, typename TraitsT>
constexpr auto basic_string_view<CharT, TraitsT>::rfind(CharT c, size_type pos) const noexcept -> size_type
{
    if (size() == 0)
    {
        return npos;
    }

    size_type idx = size() - 1;
    if (idx > pos)
    {
        idx = pos;
    }
    for (++idx; idx-- > 0;)
    {
        if (data()[idx] == c)
        {
            return idx;
        }
    }
    return npos;
}

template<typename CharT, typename TraitsT>
constexpr auto basic_string_view<CharT, TraitsT>::find_first_of(basic_string_view sv, size_type pos) const noexcept
    -> size_type
{
    if (sv.size() == 0)
    {
        return npos;
    }

    CharT const*    s = sv.data();
    size_type const n = sv.size();

    for (; pos < size(); ++pos)
    {
        if (TraitsT::find(s, n, data()[pos]) != nullptr)
        {
            return pos;
        }
    }
    return npos;
}

template<typename CharT, typename TraitsT>
constexpr auto basic_string_view<CharT, TraitsT>::find_last_of(basic_string_view sv, size_type pos) const noexcept
    -> size_type
{
    if (size() == 0 || sv.size() == 0)
    {
        return npos;
    }

    size_type idx = size();
    if (--idx > pos)
    {
        idx = pos;
    }
    do
    {
        if (TraitsT::find(sv.data(), sv.size(), data()[idx]) != nullptr)
        {
            return idx;
        }
    } while (idx-- != 0);
    return npos;
}

template<typename CharT, typename TraitsT>
constexpr auto basic_string_view<CharT, TraitsT>::find_first_not_of(basic_string_view sv, size_type pos) const noexcept
    -> size_type
{
    for (; pos < size(); ++pos)
    {
        if (TraitsT::find(sv.data(), sv.size(), data()[pos]) == nullptr)
        {
            return pos;
        }
    }
    return npos;
}

template<typename CharT, typename TraitsT>
constexpr auto basic_string_view<CharT, TraitsT>::find_first_not_of(CharT c, size_type pos) const noexcept -> size_type
{
    for (; pos < size(); ++pos)
    {
        if (data()[pos] != c)
        {
            return pos;
        }
    }
    return npos;
}

template<typename CharT, typename TraitsT>
constexpr auto basic_string_view<CharT, TraitsT>::find_last_not_of(basic_string_view sv, size_type pos) const noexcept
    -> size_type
{
    if (size() == 0)
    {
        return npos;
    }

    size_type idx = size();
    if (--idx > pos)
    {
        idx = pos;
    }
    do
    {
        if (TraitsT::find(sv.data(), sv.size(), data()[idx]) == nullptr)
        {
            return idx;
        }
    } while (idx--);
    return npos;
}

template<typename CharT, typename TraitsT>
constexpr auto basic_string_view<CharT, TraitsT>::find_last_not_of(CharT c, size_type pos) const noexcept -> size_type
{
    if (size() == 0)
    {
        return npos;
    }

    size_type idx = size();
    if (--idx > pos)
    {
        idx = pos;
    }
    do
    {
        if (data()[idx] != c)
        {
            return idx;
        }
    } while (idx--);
    return npos;
}

//
// Comparison operators
//

// operator==

// Comparison between two instances of basic_string_view
template<typename CharT, typename TraitsT>
constexpr bool operator==(basic_string_view<CharT, TraitsT> lhs, basic_string_view<CharT, TraitsT> rhs) noexcept
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }
    return lhs.compare(rhs) == 0;
}

template<typename _Type>
struct type_identity
{
    using type = _Type;
};

template<typename _Tp>
using type_identity_t = typename type_identity<_Tp>::type;

// Comparison between lhs (a basic_string_view) and rhs (a type that is implicitly convertible to basic_string_view)
template<typename CharT, typename TraitsT>
constexpr bool operator==(
    basic_string_view<CharT, TraitsT>                  lhs,
    type_identity_t<basic_string_view<CharT, TraitsT>> rhs) noexcept
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }
    return lhs.compare(rhs) == 0;
}

// Comparison between lhs (a type that is implicitly convertible to basic_string_view) and rhs (a basic_string_view)
template<typename CharT, typename TraitsT>
constexpr bool operator==(
    type_identity_t<basic_string_view<CharT, TraitsT>> lhs,
    basic_string_view<CharT, TraitsT>                  rhs) noexcept
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }
    return lhs.compare(rhs) == 0;
}

template<typename CharT, typename TraitsT>
constexpr bool operator!=(basic_string_view<CharT, TraitsT> lhs, basic_string_view<CharT, TraitsT> rhs) noexcept
{
    return !(lhs == rhs);
}

template<typename CharT, typename TraitsT>
constexpr bool operator!=(
    basic_string_view<CharT, TraitsT>                  lhs,
    type_identity_t<basic_string_view<CharT, TraitsT>> rhs) noexcept
{
    return !(lhs == rhs);
}

template<typename CharT, typename TraitsT>
constexpr bool operator!=(
    type_identity_t<basic_string_view<CharT, TraitsT>> lhs,
    basic_string_view<CharT, TraitsT>                  rhs) noexcept
{
    return !(lhs == rhs);
}

// operator<

template<typename CharT, typename TraitsT>
constexpr bool operator<(basic_string_view<CharT, TraitsT> lhs, basic_string_view<CharT, TraitsT> rhs) noexcept
{
    if (lhs.size() > rhs.size())
    {
        return false;
    }
    return lhs.compare(rhs) < 0;
}

template<typename CharT, typename TraitsT>
constexpr bool operator<(
    basic_string_view<CharT, TraitsT>                  lhs,
    type_identity_t<basic_string_view<CharT, TraitsT>> rhs) noexcept
{
    if (lhs.size() > rhs.size())
    {
        return false;
    }
    return lhs.compare(rhs) < 0;
}

template<typename CharT, typename TraitsT>
constexpr bool operator<(
    type_identity_t<basic_string_view<CharT, TraitsT>> lhs,
    basic_string_view<CharT, TraitsT>                  rhs) noexcept
{
    if (lhs.size() > rhs.size())
    {
        return false;
    }
    return lhs.compare(rhs) < 0;
}

// operator>

template<typename CharT, typename TraitsT>
constexpr bool operator>(basic_string_view<CharT, TraitsT> lhs, basic_string_view<CharT, TraitsT> rhs) noexcept
{
    if (lhs.size() < rhs.size())
    {
        return false;
    }
    return lhs.compare(rhs) > 0;
}

template<typename CharT, typename TraitsT>
constexpr bool operator>(
    basic_string_view<CharT, TraitsT>                  lhs,
    type_identity_t<basic_string_view<CharT, TraitsT>> rhs) noexcept
{
    if (lhs.size() < rhs.size())
    {
        return false;
    }
    return lhs.compare(rhs) > 0;
}

template<typename CharT, typename TraitsT>
constexpr bool operator>(
    type_identity_t<basic_string_view<CharT, TraitsT>> lhs,
    basic_string_view<CharT, TraitsT>                  rhs) noexcept
{
    if (lhs.size() < rhs.size())
    {
        return false;
    }
    return lhs.compare(rhs) > 0;
}

// operator<=

template<typename CharT, typename TraitsT>
constexpr bool operator<=(basic_string_view<CharT, TraitsT> lhs, basic_string_view<CharT, TraitsT> rhs) noexcept
{
    return !(rhs < lhs);
}

template<typename CharT, typename TraitsT>
constexpr bool operator<=(
    basic_string_view<CharT, TraitsT>                  lhs,
    type_identity_t<basic_string_view<CharT, TraitsT>> rhs) noexcept
{
    return !(rhs < lhs);
}

template<typename CharT, typename TraitsT>
constexpr bool operator<=(
    type_identity_t<basic_string_view<CharT, TraitsT>> lhs,
    basic_string_view<CharT, TraitsT>                  rhs) noexcept
{
    return !(lhs > rhs);
}

// operator>=

template<typename CharT, typename TraitsT>
constexpr bool operator>=(basic_string_view<CharT, TraitsT> lhs, basic_string_view<CharT, TraitsT> rhs) noexcept
{
    return !(lhs < rhs);
}

template<typename CharT, typename TraitsT>
constexpr bool operator>=(
    basic_string_view<CharT, TraitsT>                  lhs,
    type_identity_t<basic_string_view<CharT, TraitsT>> rhs) noexcept
{
    return !(lhs < rhs);
}

template<typename CharT, typename TraitsT>
constexpr bool operator>=(
    type_identity_t<basic_string_view<CharT, TraitsT>> lhs,
    basic_string_view<CharT, TraitsT>                  rhs) noexcept
{
    return !(lhs < rhs);
}

//
// Stream insertion operator
//

// According to the C++ standard, the traits type of the output stream must be the same as
// the one from the string_view argument. However, if the output stream's traits type is
// std::char_traits<char>, we allow our own internal traits type for the string_view
// argument as well, as it behaves exactly the same as std::char_traits<char> (except for
// being more constexpr-friendly, which is however irrelevant for this operator).
template<
    typename CharT,
    typename TraitsT,
    typename StringViewTraitsT,
    typename = std::enable_if_t<                         //
        std::is_same<TraitsT, StringViewTraitsT>::value  //
        || (std::is_same<TraitsT, std::char_traits<CharT>>::value
            && std::is_same<StringViewTraitsT, std::char_traits<CharT>>::value)  //
        >                                                                        //
    >
inline std::basic_ostream<CharT, TraitsT>& operator<<(
    std::basic_ostream<CharT, TraitsT>&         os,
    basic_string_view<CharT, StringViewTraitsT> sv)
{
    typename std::basic_ostream<CharT, TraitsT>::sentry const s(os);
    if (s)
    {
        // In case the argument's width does not fit into a std::streamsize, truncate it.
        std::size_t const max_size    = static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max());
        std::size_t const output_size = (sv.size() <= max_size) ? sv.size() : max_size;  // LCOV_EXCL_LINE cannot test

        std::streamsize const svsize       = static_cast<std::streamsize>(output_size);
        std::streamsize const stream_width = os.width();
        if (svsize < stream_width)
        {
            // Stream width is wider than text - must perform padding.
            auto fill = [&os](std::streamsize num) {
                CharT const                           ch    = os.fill();
                std::basic_streambuf<CharT, TraitsT>* rdbuf = os.rdbuf();
                while (num--)
                {
                    std::ostream::int_type const ret = rdbuf->sputc(ch);
                    if (TraitsT::eq_int_type(ret, TraitsT::eof()))
                    {
                        os.setstate(std::ios_base::badbit);
                        break;
                    }
                }
            };

            bool const is_left_adjusted = (os.flags() & std::ios_base::adjustfield) == std::ios_base::left;
            if (!is_left_adjusted)
            {
                fill(stream_width - svsize);
            }

            if (os.good())
            {
                os.write(sv.data(), svsize);
            }

            if (is_left_adjusted && os.good())
            {
                fill(stream_width - svsize);
            }
        }
        else
        {
            // Text is as long or longer than the defined stream width - just output the whole thing.
            CharT const* data = sv.data();
            os.write(data, svsize);
        }
        os.width(0);
    }

    return os;
}

}  // namespace detail

#if __cplusplus < 201703L
// External definition of a static constexpr class member.
// This is necessary in C++14 for ODR-using that value, but no longer so since C++17.
template<typename CharT, typename TraitsT>
constexpr typename stl::detail::basic_string_view<CharT, TraitsT>::size_type
    stl::detail::basic_string_view<CharT, TraitsT>::npos;
#endif

#ifdef USE_NATIVE_STRING_VIEW
#    if __cplusplus < 201703L
using string_view = fmt::basic_string_view<char>;
#    else
using string_view = std::basic_string_view<char>;
#    endif
#else
using string_view  = stl::detail::basic_string_view<char>;
using wstring_view = stl::detail::basic_string_view<wchar_t>;
#endif
}  // namespace stl

#endif  // INCLUDE_STL_STRING_VIEW_HPP
