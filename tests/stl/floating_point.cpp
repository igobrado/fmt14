#include <catch2/catch_test_macros.hpp>
#include <stl/floating_point.hpp>

TEST_CASE("Floating point 0 constructs correct object", "floating_point_tests")
{
    constexpr auto k_value = stl::make_floating_point_number(0.f);

    STATIC_REQUIRE(k_value.m_exponent == 0);
    STATIC_REQUIRE(k_value.m_significand == 0);
}

TEST_CASE("Floating point constructs correct object", "floating_point_tests")
{
    constexpr auto k_value = stl::make_floating_point_number(1.24f);

    STATIC_REQUIRE(k_value.m_exponent == -6);
    STATIC_REQUIRE(k_value.m_significand == 1240000);
}
