#include <catch2/catch_test_macros.hpp>
#include <stl/traits.hpp>

TEST_CASE("Is string type trait returns correct value", "is_string_tests")
{
    struct my_type
    {
    };

    STATIC_REQUIRE(stl::is_string<stl::string_view>::value == std::true_type::value);
    STATIC_REQUIRE(stl::is_string<char*>::value == std::true_type::value);
    STATIC_REQUIRE(stl::is_string<char const*>::value == std::true_type::value);
    STATIC_REQUIRE(stl::is_string<std::string>::value == std::true_type::value);

    STATIC_REQUIRE(stl::is_string<int>::value == std::false_type::value);
    STATIC_REQUIRE(stl::is_string<float>::value == std::false_type::value);
    STATIC_REQUIRE(stl::is_string<double>::value == std::false_type::value);
    STATIC_REQUIRE(stl::is_string<std::vector<int>>::value == std::false_type::value);
    STATIC_REQUIRE(stl::is_string<my_type>::value == std::false_type::value);
}
