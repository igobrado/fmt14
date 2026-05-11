#include <catch2/catch_test_macros.hpp>
#include <stl/fixed_buffer.hpp>

TEST_CASE("Default constructor of fixed buffer constructs default buffer", "fixed_buffer_tests")
{
    constexpr stl::fixed_buffer<std::uint8_t, 1> k_buffer{};
    STATIC_REQUIRE(k_buffer[0] == 0);
}

TEST_CASE("Move constructor intializes the buffer correctly", "fixed_buffer_tests")
{
    static constexpr std::uint8_t                     k_value = 10;
    constexpr stl::fixed_buffer<std::uint8_t, 1> k_buffer{
        stl::fixed_buffer<std::uint8_t, 1>{std::uint8_t{k_value}}};

    STATIC_REQUIRE(k_buffer[0] == k_value);
}

TEST_CASE("Aggregate constructor initializes buffer correctly", "fixed_buffer_tests")
{
    static constexpr std::size_t k_size = 5u;

    static constexpr std::uint8_t k_value_one   = 1u;
    static constexpr std::uint8_t k_value_two   = 2u;
    static constexpr std::uint8_t k_value_three = 3u;
    static constexpr std::uint8_t k_value_four  = 4u;
    static constexpr std::uint8_t k_value_five  = 5u;

    constexpr stl::fixed_buffer<std::uint8_t, k_size>
        k_buffer{k_value_one, k_value_two, k_value_three, k_value_four, k_value_five};

    STATIC_REQUIRE(k_buffer[0] == k_value_one);
    STATIC_REQUIRE(k_buffer[1] == k_value_two);
    STATIC_REQUIRE(k_buffer[2] == k_value_three);
    STATIC_REQUIRE(k_buffer[3] == k_value_four);
    STATIC_REQUIRE(k_buffer[4] == k_value_five);
    STATIC_REQUIRE(k_buffer.size() == k_size);
    STATIC_REQUIRE(k_buffer.max_size() == k_size);
    STATIC_REQUIRE(k_buffer.empty() == std::false_type::value);
}
