#ifndef INCLUDE_STL_BUFFER_HPP
#define INCLUDE_STL_BUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

namespace stl {

/**
 * @brief Constexpr friendly std::array.
 */
template<typename UnderlyingType = std::uint8_t, std::size_t N = 50>
class fixed_buffer
{
public:
    using value_type             = UnderlyingType;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;
    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /**
     * @brief Constructs empty fixed buffer.
     */
    constexpr fixed_buffer() noexcept
    : m_buffer{UnderlyingType{}}
    {
    }

    constexpr fixed_buffer(fixed_buffer const&) noexcept = delete;

    /**
     * @brief Move constructor
     *
     * @param a_other
     */
    constexpr fixed_buffer(fixed_buffer&& a_other) noexcept
    : m_buffer{}
    {
        for (size_type i = 0; i < N; ++i)
        {
            m_buffer[i] = std::move(a_other[i]);
        }
    }

    /**
     * @brief Constructs the buffer from the given set of elements.
     *
     * @param a_args
     */
    template<typename... Ts, typename = std::enable_if_t<sizeof...(Ts) <= N>>
    constexpr fixed_buffer(Ts&&... a_args) noexcept
    : m_buffer{std::forward<Ts>(a_args)...}  //
    {
    }

    /**
     * @brief Disabled copy and move assign operators
     */
    constexpr fixed_buffer& operator=(fixed_buffer const&) noexcept = delete;
    constexpr fixed_buffer& operator=(fixed_buffer&&) noexcept      = delete;

    /**
     * @brief operator []
     * @param pos
     * @return
     */
    constexpr reference operator[](size_type pos) noexcept
    {
        return m_buffer[pos];
    }

    /**
     * @brief operator []
     * @param pos
     * @return
     */
    constexpr const_reference operator[](size_type pos) const noexcept
    {
        return m_buffer[pos];
    }

    // clang-format off
    constexpr reference front() noexcept { return m_buffer[0]; }
    constexpr const_reference front() const noexcept { return m_buffer[0]; }

    constexpr reference back() noexcept { return m_buffer[N - 1]; }
    constexpr const_reference back() const noexcept { return m_buffer[N - 1]; }

    constexpr pointer data() noexcept { return m_buffer; }
    constexpr const_pointer data() const noexcept { return m_buffer; }
    constexpr const_pointer get() const noexcept { return m_buffer; } // original API
    constexpr operator const_pointer() const noexcept { return get(); }

    // -------------------
    // Iterators
    // -------------------
    constexpr iterator begin() noexcept { return m_buffer; }
    constexpr const_iterator begin() const noexcept { return m_buffer; }
    constexpr const_iterator cbegin() const noexcept { return m_buffer; }

    constexpr iterator end() noexcept { return m_buffer + N; }
    constexpr const_iterator end() const noexcept { return m_buffer + N; }
    constexpr const_iterator cend() const noexcept { return m_buffer + N; }

    constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    // -------------------
    // Capacity
    // -------------------
    static constexpr size_type size() noexcept { return N; }
    static constexpr size_type max_size() noexcept { return N; }
    static constexpr bool empty() noexcept { return N == 0; }
    // clang-format on

private:
    value_type m_buffer[N];
};

}  // namespace stl

#endif  // INCLUDE_STL_BUFFER_HPP
