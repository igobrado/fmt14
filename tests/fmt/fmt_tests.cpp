#include <catch2/catch_test_macros.hpp>
#include <fmt/fmt.hpp>

TEST_CASE("String correctly formatted", "fmt")
{
    static constexpr auto const expected = stl::string_view{"hello_world"};
    static constexpr auto const fmt_str  = fmt::format("{}", expected.data());

    STATIC_REQUIRE(fmt_str == expected);
}

TEST_CASE("Integers correctly formatted", "fmt")
{
    static constexpr auto const expected = stl::string_view{"1, 2, 3, 4"};
    static constexpr auto const fmt_str  = fmt::format("{}, {}, {}, {}", 1, 2, 3, 4);

    STATIC_REQUIRE(fmt_str == expected);
}

TEST_CASE("Special characters not ignored", "fmt")
{
    static constexpr auto const expected = stl::string_view{"1, 2, 3, 4, !#$%&/()=?*"};
    static constexpr auto const fmt_str  = fmt::format("{}, {}, {}, {}, !#$%&/()=?*", 1, 2, 3, 4);

    STATIC_REQUIRE(fmt_str == expected);
}

TEST_CASE("Floats correctly formatted", "fmt")
{
    static constexpr auto const expected = stl::string_view{"1.230000"};
    static constexpr auto const fmt_str  = fmt::format("{}", 1.23f);

    STATIC_REQUIRE(fmt_str == expected);
}

TEST_CASE()
{
    static constexpr auto const expected = stl::string_view{"1234132"};
    static constexpr auto const tt       = fmt::fixed_format_string{"1234", 1, "32"};

    STATIC_REQUIRE(expected == tt);
}
